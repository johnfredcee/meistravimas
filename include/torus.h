#ifndef TORUS_H
#define TORUS_H

namespace venk {

class TorusBuilder {
  std::shared_ptr<BufferBuilder> vertexBuffer;
  std::shared_ptr<BufferBuilder> normalBuffer;
  std::shared_ptr<BufferBuilder> uvBuffer;
  std::shared_ptr<BufferBuilder> indexBuffer;
public:
  TorusBuilder(float radiusU, float radiusV, Uint32 numSegU, Uint32 numSegV);  
  std::shared_ptr<BufferBuilder> getVertices() { return vertexBuffer; };
  std::shared_ptr<BufferBuilder> getNormals() { return normalBuffer; };
  std::shared_ptr<BufferBuilder> getUVs() { return uvBuffer; };
  std::shared_ptr<BufferBuilder> getIndices() { return indexBuffer; };
};

}

#endif

