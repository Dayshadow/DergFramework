#ifndef MESH_H
#define MESH_H
#include <GL/glew.h>
#include <vector>
#include <initializer_list>
#include "Framework/Log.hpp"
#include "GlCheck.hpp"
#include "GlIDs.hpp"
#include <assert.h>


enum MeshBehavior : uint32_t {
    NO_VAO_INIT = 0b1,
    VAO_INIT = 0b10,
    FEEDBACK_MESH = 0b100
};

// Holds the attribute schema to allow for arbitrary type and size.
struct Attrib {
    Attrib(GLuint p_size, GLenum p_type) {
        size = p_size;
        type = p_type;
    }
    Attrib(GLuint p_size, GLenum p_type, bool p_instanced) {
        size = p_size;
        type = p_type;
        instanced = p_instanced;
    }
    bool instanced = false;
    GLuint size;
    GLenum type;
};

/// A container for managing vertex buffers and VAOs, allows easy attribute assigning and more
/// Templated for the vertex data being passed in, capable of holding a combination of float/int in a packed struct as well as single types. Only supports float and uint for now.
/// Trust me it's much less complicated if I move all the .cpp code into the .hpp file. shush.
/// T: vertex attribute data type, can be a struct
/// I: instance attribute data type, can be a struct
template <class T, class I = int>
class Mesh
{
public:
    // THESE MEAN THAT THE OPENGL OBJECT HAS BEEN CREATED. IT DOES NOT MEAN IT NEEDS TO HAVE DATA.
    bool VAOInitialized = false;
    bool VBOInitialized = false;
    bool IBOInitialized = false;
    bool usingInstancing = false;
    bool instancesInitialized = false;

    // Manage our opengl memory automatically
    std::unique_ptr<glBuffer> vert_VBO = std::make_unique<glBuffer>();
    std::unique_ptr<glBuffer> inst_VBO = std::make_unique<glBuffer>();
    std::unique_ptr<glVertexArray> VAO = std::make_unique<glVertexArray>();
    std::unique_ptr<glBuffer> IBO = std::make_unique<glBuffer>();

    Mesh()
    {
        glGenVertexArrays(1, &VAO->ID);
        GLGEN_LOG("Generated Vertex Array " << VAO->ID);
        VAOInitialized = true;
    }

    Mesh(MeshBehavior behavior)
    {
        if (!(behavior & MeshBehavior::NO_VAO_INIT)) {
            glGenVertexArrays(1, &VAO->ID);
            GLGEN_LOG("Generated Vertex Array " << VAO->ID);
            VAOInitialized = true;
        }
        if (behavior & MeshBehavior::FEEDBACK_MESH) isFeedbackMesh = true;
    }

    // Mesh copying is a pretty heavy operation, so try not to
    Mesh(const Mesh& p_other) :
        m_verts(p_other.m_verts),
        m_indices(p_other.m_indices),
        m_instances(p_other.m_instances),
        m_attribList(p_other.m_attribList),
        m_singleVertexSize(p_other.m_singleVertexSize),
        m_singleInstanceSize(p_other.m_singleInstanceSize),
        m_GPUVertCount(p_other.m_GPUVertCount),
        m_streamType(p_other.m_streamType)
    {
        // We can't copy these, so we have to create new opengl buffers. Copying is discouraged.
        GLGEN_LOG("Copied mesh! VAO: " << p_other.VAO->ID);
        vert_VBO = std::make_unique<glBuffer>();
        inst_VBO = std::make_unique<glBuffer>();
        IBO = std::make_unique<glBuffer>();
        VBOInitialized = false;
        IBOInitialized = false;
        instancesInitialized = false;
        VAO = std::make_unique<glVertexArray>();
        glGenVertexArrays(1, &VAO->ID);
        GLGEN_LOG("Generated Vertex Array " << VAO->ID);
    }

    Mesh<T, I>& operator=(const Mesh<T, I>& p_other) {
        m_verts = p_other.m_verts;
        m_indices = p_other.m_indices;
        m_instances = p_other.m_instances;
        m_attribList = p_other.m_attribList;
        m_singleVertexSize = p_other.m_singleVertexSize;
        m_singleInstanceSize = p_other.m_singleInstanceSize;
        m_GPUVertCount = p_other.m_GPUVertCount;
        m_streamType = p_other.m_streamType;

        // We can't copy these, so we have to create new opengl buffers. Copying is discouraged.
        GLGEN_LOG("Copied mesh! VAO: " << p_other.VAO->ID);
        vert_VBO = std::make_unique<glBuffer>();
        inst_VBO = std::make_unique<glBuffer>();
        IBO = std::make_unique<glBuffer>();
        VBOInitialized = false;
        IBOInitialized = false;
        instancesInitialized = false;
        VAO = std::make_unique<glVertexArray>();
        glGenVertexArrays(1, &VAO->ID);
        GLGEN_LOG("Generated Vertex Array " << VAO->ID);

        return *this;
    }

    Mesh<T, I>& operator=(Mesh<T, I>&& p_other) noexcept {
        m_verts = p_other.m_verts;
        m_indices = p_other.m_indices;
        m_instances = p_other.m_instances;
        m_attribList = p_other.m_attribList;
        m_singleVertexSize = p_other.m_singleVertexSize;
        m_singleInstanceSize = p_other.m_singleInstanceSize;
        m_GPUVertCount = p_other.m_GPUVertCount;
        m_GPUIndicesCount = p_other.m_GPUIndicesCount;
        m_streamType = p_other.m_streamType;
        VAOInitialized = p_other.VAOInitialized;
        VBOInitialized = p_other.VBOInitialized;
        IBOInitialized = p_other.IBOInitialized;
        instancesInitialized = p_other.instancesInitialized;
        usingInstancing = p_other.usingInstancing;

        GLGEN_LOG("Moved mesh! VAO: " << p_other.VAO->ID);

        vert_VBO = std::move(p_other.vert_VBO);
        inst_VBO = std::move(p_other.inst_VBO);
        VAO = std::move(p_other.VAO);
        IBO = std::move(p_other.IBO);

        return *this;
    }

    // Move constructor for rvalues
    Mesh(Mesh&& p_other) noexcept :
        m_verts(p_other.m_verts),
        m_indices(p_other.m_indices),
        m_instances(p_other.m_instances),
        m_attribList(p_other.m_attribList),
        m_singleVertexSize(p_other.m_singleVertexSize),
        m_singleInstanceSize(p_other.m_singleInstanceSize),
        m_GPUVertCount(p_other.m_GPUVertCount),
        m_GPUIndicesCount(p_other.m_GPUIndicesCount),
        m_streamType(p_other.m_streamType),
        VAOInitialized(p_other.VAOInitialized),
        VBOInitialized(p_other.VBOInitialized),
        IBOInitialized(p_other.IBOInitialized),
        instancesInitialized(p_other.instancesInitialized),
        usingInstancing(p_other.usingInstancing)
    {
        GLGEN_LOG("Moved mesh! VAO: " << p_other.VAO->ID);

        vert_VBO = std::move(p_other.vert_VBO);
        inst_VBO = std::move(p_other.inst_VBO);
        VAO = std::move(p_other.VAO);
        IBO = std::move(p_other.IBO);

    }
    /// Allows you to skip a few memory allocations if you reserve it right off the bat.
    void reserve(size_t p_reserveAmount) { m_verts.reserve(p_reserveAmount); }

    void setStreamType(GLenum p_type) {
#ifdef SBBB_DEBUG
        if (!(p_type == GL_STREAM_DRAW || p_type == GL_DYNAMIC_DRAW || p_type == GL_STATIC_DRAW)) {
            throw std::runtime_error("Used an invalid enum to set a stream type.");
            return;
        }
#endif
        m_streamType = p_type;
    }

    /// Size specifies number of floats required for the attribute.
    void addFloatAttrib(GLuint p_size, bool isInstanced = false) {
        usingInstancing |= isInstanced;
        if (!isInstanced)
            m_singleVertexSize += sizeof(GLfloat) * p_size;
        else
            m_singleInstanceSize += sizeof(GLfloat) * p_size;
        m_attribList.emplace_back(p_size, GL_FLOAT, isInstanced);
    };

    /// Size specifies number of uints required for the attribute.
    void addUintAttrib(GLuint p_size, bool isInstanced = false) {
        usingInstancing |= isInstanced;
        if (!isInstanced)
            m_singleVertexSize += sizeof(GLuint) * p_size;
        else
            m_singleInstanceSize += sizeof(GLuint) * p_size;
        m_attribList.emplace_back(p_size, GL_UNSIGNED_INT, isInstanced);
    };

    /// Size specifies COUNT of bytes required for the attribute. as if it's an array
    void addUbyteAttrib(GLubyte p_size, bool isInstanced = false) {
        usingInstancing |= isInstanced;
        // I don't think things can be smaller than 4 bytes, so the size of a ubyte is actually 4 on the gpu
        if (!isInstanced)
            m_singleVertexSize += sizeof(GLuint) * p_size;
        else
            m_singleInstanceSize += sizeof(GLuint) * p_size;
        m_attribList.emplace_back(p_size, GL_UNSIGNED_BYTE, isInstanced);
    };

    void addIntAttrib(GLubyte p_size, bool isInstanced = false) {
        usingInstancing |= isInstanced;
        if (!isInstanced)
            m_singleVertexSize += sizeof(GLint) * p_size;
        else
            m_singleInstanceSize += sizeof(GLint) * p_size;
        m_attribList.emplace_back(p_size, GL_INT, isInstanced);
    };

    // only use if type T is a struct, or represents a vert with only one attribute
    void pushVertex(T&& p_vert) {
        m_verts.push_back(p_vert);
    }
    // only use if type T is a struct, or represents a vert with only one attribute
    void pushVertex(T& p_vert) {
        m_verts.push_back(p_vert);
    }
    /// Simply adds a vertex of type T to the end of the mesh list.
    // make sure you push a multiple of the total vert size worth of data.
    void pushVertices(const std::initializer_list<T>& p_attribs) {
        m_verts.insert(m_verts.end(), p_attribs);
    };

    // only use if type T is a struct, or represents a instance with only one attribute
    void pushInstance(I&& p_instance) {
        m_instances.push_back(p_instance);
    }
    void setInstance(size_t p_index, I& p_value) {
        assert(p_index < m_instances.size());
        m_instances[p_index] = p_value;
    }
    void setInstance(size_t p_index, I&& p_value) {
        assert(p_index < m_instances.size());
        m_instances[p_index] = p_value;
    }
    void pushInstance(I& p_instance) {
        m_instances.push_back(p_instance);
    }
    /// type I must be a complete instance, no attrib splitting
    void pushInstances(const std::initializer_list<I>& p_attribs) {
        m_instances.insert(m_instances.end(), p_attribs);
    };

    void pushIndices(const std::initializer_list<GLuint>& p_attribs) {
        m_indices.insert(m_indices.end(), p_attribs);
    }

    // make sure you set attributes beforehand.
    void initFeedbackBuffer(uint32_t maxElementCount, GLuint p_VAO) {
        if (!isFeedbackMesh) ERROR_LOG("Trying to do feedback init on a non-feedback mesh");
        if (VBOInitialized) ERROR_LOG("VBO has already been created by something else. Can't re-init");

        if (!VAOInitialized) {
            glGenVertexArrays(1, &VAO->ID);
            GLGEN_LOG("Generated Vertex Array " << VAO->ID);
            VAOInitialized = true;
        }
        if (!instancesInitialized && usingInstancing) {
            glGenBuffers(1, &inst_VBO->ID);
            GLGEN_LOG("Generated GPU Instancing Buffer " << inst_VBO->ID);
            instancesInitialized = true;
        }

        glBindVertexArray(VAO->ID);

        glGenBuffers(1, &vert_VBO->ID);
        GLGEN_LOG("Generated Vertex Feedback Buffer " << vert_VBO->ID);
        VBOInitialized = true;

        glCheck(glBindBuffer(GL_ARRAY_BUFFER, vert_VBO->ID));

        setAttribPointers();

        if (usingInstancing) {
            glCheck(glBindBuffer(GL_ARRAY_BUFFER, inst_VBO->ID));
            glCheck(glBufferData(GL_ARRAY_BUFFER, sizeof(I) * m_instances.size(), m_instances.data(), m_streamType));

            setInstancePointers();
        }

        maxFeedbackSize = maxElementCount * sizeof(T);
        glCheck(glBufferData(GL_ARRAY_BUFFER, maxFeedbackSize, nullptr, GL_DYNAMIC_DRAW));

        //glCheck(glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0));
        feedbackInitDone = true;
    }

    GLuint getCapturedPrimitiveCount() {
        return capturedPrimitiveCount;
    }
    void startFeedback(GLenum primitiveType) {
        glBindVertexArray(VAO->ID);

        glCheck(glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vert_VBO->ID));

        glCheck(glBeginTransformFeedback(primitiveType));

        glGenQueries(1, &primitiveQuery);
        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, primitiveQuery);

        glCheck(glBindVertexArray(0));
    }

    void endFeedback() {
        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
        glGetQueryObjectuiv(primitiveQuery, GL_QUERY_RESULT, &capturedPrimitiveCount);
        glDeleteQueries(1, &primitiveQuery);

        glCheck(glEndTransformFeedback());
        glCheck(glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0));
    }
    bool hasData() { return (bool)m_verts.size(); };
    // confusing, but it's the count of attributes that were sent to the gpu at some point.
    size_t getTotalVBOSize() { return m_GPUVertCount; }
    size_t getTotalIBOSize() { return m_GPUIndicesCount; }
    size_t getStoredVertCount() { return m_verts.size(); }
    size_t instanceCount() { return m_instances.size(); }
    GLuint* getIBOPointer() { return m_indices.data(); }
    I* getInstancePointer(size_t p_index = 0) { return &m_instances[p_index]; };

    void pushVBOToGPU() {
        if (isFeedbackMesh) return;
        if (!VAOInitialized) {
            glGenVertexArrays(1, &VAO->ID);
            GLGEN_LOG("Generated Vertex Array " << VAO->ID);
            VAOInitialized = true;
        }
        glBindVertexArray(VAO->ID);
        if (!VBOInitialized) {
            glGenBuffers(1, &vert_VBO->ID);
            GLGEN_LOG("Generated Vertex Buffer " << vert_VBO->ID);
            VBOInitialized = true;
        }
        else {
            GLGEN_LOG("Re-generated Vertex Buffer " << vert_VBO->ID);
        }

        glCheck(glBindBuffer(GL_ARRAY_BUFFER, vert_VBO->ID));
        glCheck(glBufferData(GL_ARRAY_BUFFER, sizeof(T) * m_verts.size(), m_verts.data(), m_streamType));

        setAttribPointers();

        glEnableVertexAttribArray(0);

        // keep it thread-accurate
        m_GPUVertCount = (m_verts.size() * sizeof(T)) / m_singleVertexSize;
        m_GPUIndicesCount = m_indices.size();
    };

    void pushInstancesToGPU() {
        if (!VAOInitialized) {
            glGenVertexArrays(1, &VAO->ID);
            GLGEN_LOG("Generated Vertex Array " << VAO->ID);
            VAOInitialized = true;
        }
        glBindVertexArray(VAO->ID);
        if (!instancesInitialized && usingInstancing) {
            glGenBuffers(1, &inst_VBO->ID);
            GLGEN_LOG("Generated GPU Instancing Buffer " << inst_VBO->ID);
            instancesInitialized = true;
        }
        else {
            GLGEN_LOG("Re-generated Instance Buffer " << inst_VBO->ID);
        }

        if (usingInstancing) {
            glCheck(glBindBuffer(GL_ARRAY_BUFFER, inst_VBO->ID));
            glCheck(glBufferData(GL_ARRAY_BUFFER, sizeof(I) * m_instances.size(), m_instances.data(), m_streamType));

            setInstancePointers();
        }

        glEnableVertexAttribArray(0);
    }
    void pushIBOToGPU() {
        if (isFeedbackMesh) return;
        if (!VAOInitialized) {
            glGenVertexArrays(1, &VAO->ID);
            GLGEN_LOG("Generated Vertex Array " << VAO->ID);
            VAOInitialized = true;
        }
        if (m_indices.size() == 0) return;
        glCheck(glBindVertexArray(VAO->ID));
        if (!IBOInitialized) {
            glGenBuffers(1, &IBO->ID);
            GLGEN_LOG("Generated Index Buffer " << IBO->ID);
            IBOInitialized = true;
        }
        glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO->ID));
        glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * m_indices.size(), m_indices.data(), m_streamType));
        glEnableVertexAttribArray(GL_NONE);
    }
    void subCurrentVBOData() {
        if (isFeedbackMesh) return;
        glCheck(glBindVertexArray(VAO->ID));
        if (!VBOInitialized) {
            glGenBuffers(1, &vert_VBO->ID);
            GLGEN_LOG("Generated Vertex Buffer " << vert_VBO->ID);
            VBOInitialized = true;
        }
        glCheck(glBindBuffer(GL_ARRAY_BUFFER, vert_VBO->ID));
        assert((m_verts.size() * sizeof(T)) / m_singleVertexSize == m_GPUVertCount);
        glCheck(glBufferSubData(GL_ARRAY_BUFFER, 0, m_verts.size() * sizeof(T), m_verts.data()));
        glBindVertexArray(0);
    }
    // NOTE: endIndex is not inclusive, so setting start and end to the same value will not affect any data, make sure end is at least one greater than start
    void subVBOData(GLuint p_startIndex, GLuint p_endIndex, T* p_data) {
        if (isFeedbackMesh) return;
        glCheck(glBindVertexArray(VAO->ID));
        if (!VBOInitialized) {
            glGenBuffers(1, &vert_VBO->ID);
            GLGEN_LOG("Generated Vertex Buffer " << vert_VBO->ID);
            VBOInitialized = true;
        }
        glCheck(glBindBuffer(GL_ARRAY_BUFFER, vert_VBO->ID));
        glCheck(glBufferSubData(GL_ARRAY_BUFFER, p_startIndex * sizeof(T), (p_endIndex - p_startIndex) * sizeof(T), p_data));
        glBindVertexArray(0);
    }
    void subCurrentInstanceData() {
        if (!instancesInitialized || !usingInstancing) {
            ERROR_LOG("You forgot to give the mesh any instances in the first place.");
            return;
        }
        glCheck(glBindVertexArray(VAO->ID));
        glCheck(glBindBuffer(GL_ARRAY_BUFFER, inst_VBO->ID));
        // I guess make sure that the single instance size is correct
        assert((m_instances.size() * sizeof(I)) / m_singleInstanceSize == instanceCount());
        glCheck(glBufferSubData(GL_ARRAY_BUFFER, 0, m_instances.size() * sizeof(I), m_instances.data()));
        glBindVertexArray(0);
    }
    // NOTE: endIndex is not inclusive, so setting start and end to the same value will not affect any data, make sure end is at least one greater than start
    void subInstanceData(GLuint p_startIndex, GLuint p_endIndex, I* p_data) {
        if (!instancesInitialized || !usingInstancing) {
            ERROR_LOG("You forgot to give the mesh any instances in the first place.");
            return;
        }
        glCheck(glBindVertexArray(VAO->ID));
        glCheck(glBindBuffer(GL_ARRAY_BUFFER, inst_VBO->ID));
        glCheck(glBufferSubData(GL_ARRAY_BUFFER, p_startIndex * sizeof(I), (p_endIndex - p_startIndex) * sizeof(I), p_data));
    }

    void resetGPUCounts() {
        m_GPUVertCount = 0;
        m_GPUIndicesCount = 0;
    }
    // dealloc the cpu-side verts, but keep gpu state the same
    void clean() {
        m_verts.clear();
        m_verts.shrink_to_fit();
        m_indices.clear();
        m_indices.shrink_to_fit();
        m_instances.clear();
        m_instances.shrink_to_fit();
    }
    void remove() {
        m_verts = std::vector<T>();
        m_indices = std::vector<GLuint>();
        m_instances = std::vector<I>();
        vert_VBO->~glBuffer();
        inst_VBO->~glBuffer();
        IBO->~glBuffer();
        VBOInitialized = false;
        IBOInitialized = false;
        usingInstancing = false;
        instancesInitialized = false;
    };

    std::vector<T>& getVerts() {
        return m_verts;
    }
    bool isFeedbackMesh = false;
    bool feedbackInitDone = false;

private:

    void setAttribPointers() {
        uint64_t currentOffset = 0; // Offset that needs to be updated as arbitrary amounts of attributes are added. 
        for (uint32_t i = 0; i < m_attribList.size(); i++) {
            if (m_attribList[i].instanced) continue; // ignore instanced verts, they go in a different vbo

            switch (m_attribList[i].type) { // Will add more attribute types as I need them. 
            case GL_FLOAT:
                glCheck(glVertexAttribPointer(i, m_attribList[i].size, GL_FLOAT, GL_FALSE, m_singleVertexSize, reinterpret_cast<const void*>(currentOffset)));
                currentOffset += (GLint)(m_attribList[i].size * sizeof(GLfloat));
                break;
            case GL_UNSIGNED_INT:
                glCheck(glVertexAttribIPointer(i, m_attribList[i].size, GL_UNSIGNED_INT, m_singleVertexSize, reinterpret_cast<const void*>(currentOffset)));
                currentOffset += (GLint)(m_attribList[i].size * sizeof(GLuint));
                break;
            case GL_INT:
                glCheck(glVertexAttribIPointer(i, m_attribList[i].size, GL_INT, m_singleVertexSize, reinterpret_cast<const void*>(currentOffset)));
                currentOffset += (GLint)(m_attribList[i].size * sizeof(GLint));
                break;
            case GL_UNSIGNED_BYTE:
                glCheck(glVertexAttribIPointer(i, m_attribList[i].size, GL_UNSIGNED_BYTE, m_singleVertexSize, reinterpret_cast<const void*>(currentOffset)));
                currentOffset += (GLint)(m_attribList[i].size * sizeof(GLuint));
                break;
            default:
                assert("unreachable" && 0);
                break;
            }
            glCheck(glEnableVertexAttribArray(i));
        }
    }

    void setInstancePointers() {
        uint64_t currentOffset = 0; // Offset that needs to be updated as arbitrary amounts of attributes are added. 
        for (uint32_t i = 0; i < m_attribList.size(); i++) {
            if (!m_attribList[i].instanced) continue; // ignore non-instanced verts

            switch (m_attribList[i].type) { // Will add more attribute types as I need them. 
            case GL_FLOAT:
                glCheck(glVertexAttribPointer(i, m_attribList[i].size, GL_FLOAT, GL_FALSE, m_singleInstanceSize, reinterpret_cast<const void*>(currentOffset)));
                currentOffset += (GLint)(m_attribList[i].size * sizeof(GLfloat));
                break;
            case GL_UNSIGNED_INT:
                glCheck(glVertexAttribIPointer(i, m_attribList[i].size, GL_UNSIGNED_INT, m_singleInstanceSize, reinterpret_cast<const void*>(currentOffset)));
                currentOffset += (GLint)(m_attribList[i].size * sizeof(GLuint));
                break;
            case GL_INT:
                glCheck(glVertexAttribIPointer(i, m_attribList[i].size, GL_INT, m_singleInstanceSize, reinterpret_cast<const void*>(currentOffset)));
                currentOffset += (GLint)(m_attribList[i].size * sizeof(GLint));
                break;
            case GL_UNSIGNED_BYTE:
                glCheck(glVertexAttribIPointer(i, m_attribList[i].size, GL_UNSIGNED_BYTE, m_singleInstanceSize, reinterpret_cast<const void*>(currentOffset)));
                currentOffset += (GLint)(m_attribList[i].size * sizeof(GLuint));
                break;
            default:
                assert("unreachable" && 0);
                break;
            }
            glCheck(glEnableVertexAttribArray(i));
            // only update on unique instance
            glCheck(glVertexAttribDivisor(i, 1));
        }
    }

    std::vector<T> m_verts; // Vert data in any container you deem fit.
    std::vector<GLuint> m_indices;
    std::vector<I> m_instances;

    // feedback mesh stuff
    GLuint primitiveQuery = 0;
    uint32_t capturedPrimitiveCount = 0;
    GLuint maxFeedbackSize = 0;

    std::vector<Attrib> m_attribList; // List of attributes the shader uses.
    uint32_t m_singleVertexSize = 0;
    uint32_t m_singleInstanceSize = 0;
    // used so I don't have to keep the verts around on system memory
    uint32_t m_GPUVertCount = 0;
    uint32_t m_GPUIndicesCount = 0;

    // By default, vbo data is expected not to change. Be sure to set it to dynamic if that is not the case.
    GLenum m_streamType = GL_STATIC_DRAW;
};

#endif