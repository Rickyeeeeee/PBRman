#include "Mesh.h"
#include <fstream>
#include <tinyply.h>

static inline std::vector<uint8_t> read_file_binary(const std::string & pathToFile)
{
    std::ifstream file(pathToFile, std::ios::binary);
    std::vector<uint8_t> fileBufferBytes;

    if (file.is_open())
    {
        file.seekg(0, std::ios::end);
        size_t sizeBytes = file.tellg();
        file.seekg(0, std::ios::beg);
        fileBufferBytes.resize(sizeBytes);
        if (file.read((char*)fileBufferBytes.data(), sizeBytes)) return fileBufferBytes;
    }
    else throw std::runtime_error("could not open binary ifstream to path " + pathToFile);
    return fileBufferBytes;
}

Mesh::Mesh(const std::string& plyFIlePath)
        : m_PlyFilePath(plyFIlePath)
{
    std::cout << "........................................................................\n";
    std::cout << "Now Reading: " << m_PlyFilePath << std::endl;

    std::unique_ptr<std::istream> file_stream;
    std::vector<uint8_t> byte_buffer;

    bool preload_into_memory = true;

    try
    {
        // For most files < 1gb, pre-loading the entire file upfront and wrapping it into a 
        // stream is a net win for parsing speed, about 40% faster. 
        // if (preload_into_memory)
        // {
        //     byte_buffer = read_file_binary(m_PlyFilePath);
        //     file_stream.reset(new memory_stream((char*)byte_buffer.data(), byte_buffer.size()));
        // }
        // else
        {
            file_stream.reset(new std::ifstream(m_PlyFilePath, std::ios::binary));
        }

        if (!file_stream || file_stream->fail()) throw std::runtime_error("file_stream failed to open " + m_PlyFilePath);

        file_stream->seekg(0, std::ios::end);
        const float size_mb = file_stream->tellg() * float(1e-6);
        file_stream->seekg(0, std::ios::beg);

        tinyply::PlyFile file;
        file.parse_header(*file_stream);

        std::cout << "\t[ply_header] Type: " << (file.is_binary_file() ? "binary" : "ascii") << std::endl;
        for (const auto & c : file.get_comments()) std::cout << "\t[ply_header] Comment: " << c << std::endl;
        for (const auto & c : file.get_info()) std::cout << "\t[ply_header] Info: " << c << std::endl;

        for (const auto & e : file.get_elements())
        {
            std::cout << "\t[ply_header] element: " << e.name << " (" << e.size << ")" << std::endl;
            for (const auto & p : e.properties)
            {
                std::cout << "\t[ply_header] \tproperty: " << p.name << " (type=" << tinyply::PropertyTable[p.propertyType].str << ")";
                if (p.isList) std::cout << " (list_type=" << tinyply::PropertyTable[p.listType].str << ")";
                std::cout << std::endl;
            }
        }

        // Because most people have their own mesh types, tinyply treats parsed data as structured/typed byte buffers. 
        // See examples below on how to marry your own application-specific data structures with this one. 
        std::shared_ptr<tinyply::PlyData> vertices, normals, colors, texcoords, faces, tripstrip;

        // The header information can be used to programmatically extract properties on elements
        // known to exist in the header prior to reading the data. For brevity of this sample, properties 
        // like vertex position are hard-coded: 
        try { vertices = file.request_properties_from_element("vertex", { "x", "y", "z" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { normals = file.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        // try { colors = file.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }); }
        // catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        // try { colors = file.request_properties_from_element("vertex", { "r", "g", "b", "a" }); }
        // catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        // try { texcoords = file.request_properties_from_element("vertex", { "u", "v" }); }
        // catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        // Providing a list size hint (the last argument) is a 2x performance improvement. If you have 
        // arbitrary ply files, it is best to leave this 0. 
        try { faces = file.request_properties_from_element("face", { "vertex_indices" }, 3); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        // Tristrips must always be read with a 0 list size hint (unless you know exactly how many elements
        // are specifically in the file, which is unlikely); 
        // try { tripstrip = file.request_properties_from_element("tristrips", { "vertex_indices" }, 0); }
        // catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        file.read(*file_stream);

        if (vertices)   std::cout << "\tRead " << vertices->count  << " total vertices "<< std::endl;
        if (normals)    std::cout << "\tRead " << normals->count   << " total vertex normals " << std::endl;
        if (colors)     std::cout << "\tRead " << colors->count << " total vertex colors " << std::endl;
        if (texcoords)  std::cout << "\tRead " << texcoords->count << " total vertex texcoords " << std::endl;
        if (faces)      std::cout << "\tRead " << faces->count     << " total faces (triangles) " << std::endl;
        if (tripstrip)  std::cout << "\tRead " << (tripstrip->buffer.size_bytes() / tinyply::PropertyTable[tripstrip->t].stride) << " total indices (tristrip) " << std::endl;

        {
            const size_t numVerticesBytes = vertices->buffer.size_bytes();
            m_Vertices.resize(vertices->count);
            std::memcpy(m_Vertices.data(), vertices->buffer.get(), numVerticesBytes);

            if (normals && normals->count == m_Vertices.size())
            {
                const size_t numNormalsBytes = normals->buffer.size_bytes();
                m_Normals.resize(normals->count);
                std::memcpy(m_Normals.data(), normals->buffer.get(), numNormalsBytes);
            }
            else
            {
                std::cout << "[Error]: Normals count != Vertices count" << std::endl;
            }

            if (faces)
            {
                // faces->buffer contains indices, usually as uint32_t or uint16_t
                size_t indexCount = faces->count * 3; // assuming triangles
                if (faces->t == tinyply::Type::UINT32) {
                    m_TriangleIndices.resize(indexCount);
                    std::memcpy(m_TriangleIndices.data(), faces->buffer.get(), indexCount * sizeof(uint32_t));
                } else if (faces->t == tinyply::Type::UINT16) {
                    std::vector<uint16_t> temp(indexCount);
                    std::memcpy(temp.data(), faces->buffer.get(), indexCount * sizeof(uint16_t));
                    m_TriangleIndices.assign(temp.begin(), temp.end());
                } else if (faces->t == tinyply::Type::INT32) {
                    std::vector<int32_t> temp(indexCount);
                    std::memcpy(temp.data(), faces->buffer.get(), indexCount * sizeof(int32_t));
                    m_TriangleIndices.assign(temp.begin(), temp.end());
                }
                // Add more types if needed
            }
            else
            {
                std::cout << "[Error]: Normals count != Vertices count" << std::endl;
            }
            

            if (texcoords && texcoords->count == m_Vertices.size()) 
            {
                const size_t numTexcoordBytes = texcoords->buffer.size_bytes();
                m_TexCoords.resize(texcoords->count);
                std::memcpy(m_TexCoords.data(), texcoords->buffer.get(), numTexcoordBytes);
            }
            else
            {
                std::cout << "[Error]: TexCoord count != Vertices count" << std::endl;
            }
        }

        // Example One: converting to your own application types
        // {
        //     const size_t numVerticesBytes = vertices->buffer.size_bytes();
        //     std::vector<float3> verts(vertices->count);
        //     std::memcpy(verts.data(), vertices->buffer.get(), numVerticesBytes);
        // }

        // Example Two: converting to your own application type
        // {
        //     std::vector<float3> verts_floats;
        //     std::vector<double3> verts_doubles;
        //     if (vertices->t == tinyply::Type::FLOAT32) { /* as floats ... */ }
        //     if (vertices->t == tinyply::Type::FLOAT64) { /* as doubles ... */ }
        // }
    }
    catch (const std::exception & e)
    {
        std::cerr << "Caught tinyply exception: " << e.what() << std::endl;
    }        
}
