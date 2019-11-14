#pragma once

#include "renderbuffer.hpp"



namespace cges {

class Texture : public RenderBuffer{
public:
  Texture(const size_t width, const size_t height);

  // ������true, ���s��false��Ԃ�
  bool LoadPpm(const char* const filePath);

  // �e�N�X�`�����W�𕂓������Ŏw�肵�ĐF���擾
  ColorRGBA GetPixel(const float x, const float y);
};

}// namespace cges