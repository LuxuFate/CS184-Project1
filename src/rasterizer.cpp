#include "rasterizer.h"

using namespace std;

namespace CGL {

  RasterizerImp::RasterizerImp(PixelSampleMethod psm, LevelSampleMethod lsm,
    size_t width, size_t height,
    unsigned int sample_rate) {
    this->psm = psm;
    this->lsm = lsm;
    this->width = width;
    this->height = height;
    this->sample_rate = sample_rate;

    sample_buffer.resize(width * height * sample_rate, Color::White);
  }

  // Used by rasterize_point and rasterize_line
  void RasterizerImp::fill_pixel(size_t x, size_t y, Color c) {
    // TODO: Task 2: You might need to this function to fix points and lines (such as the black rectangle border in test4.svg)
    // NOTE: You are not required to implement proper supersampling for points and lines
    // It is sufficient to use the same color for all supersamples of a pixel for points and lines (not triangles)

    rgb_framebuffer_target[3 * (y * width + x)] = (unsigned char)(c.r * 255);
    rgb_framebuffer_target[3 * (y * width + x) + 1] = (unsigned char)(c.g * 255);
    rgb_framebuffer_target[3 * (y * width + x) + 2] = (unsigned char)(c.b * 255);
    sample_buffer[sqrt(sample_rate) * (y * width + x)] = c;
  }

  // Rasterize a point: simple example to help you start familiarizing
  // yourself with the starter code.
  //
  void RasterizerImp::rasterize_point(float x, float y, Color color) {
    // fill in the nearest pixel
    int sx = (int)floor(x);
    int sy = (int)floor(y);

    // check bounds
    if (sx < 0 || sx >= width) return;
    if (sy < 0 || sy >= height) return;

    fill_pixel(sx, sy, color);
    return;
  }

  // Rasterize a line.
  void RasterizerImp::rasterize_line(float x0, float y0,
    float x1, float y1,
    Color color) {
    if (x0 > x1) {
      swap(x0, x1); swap(y0, y1);
    }

    float pt[] = { x0,y0 };
    float m = (y1 - y0) / (x1 - x0);
    float dpt[] = { 1,m };
    int steep = abs(m) > 1;
    if (steep) {
      dpt[0] = x1 == x0 ? 0 : 1 / abs(m);
      dpt[1] = x1 == x0 ? (y1 - y0) / abs(y1 - y0) : m / abs(m);
    }

    while (floor(pt[0]) <= floor(x1) && abs(pt[1] - y0) <= abs(y1 - y0)) {
      rasterize_point(pt[0], pt[1], color);
      pt[0] += dpt[0]; pt[1] += dpt[1];
    }
  }

float eq(float xa, float ya,
         float xb, float yb,
         float xc, float yc) {
    return -(xb-xa)*(yc-ya) + (yb-ya)*(xc-xa);
}

bool insideTriangle(float x, float y,
                    float x0, float y0,
                    float x1, float y1,
                    float x2, float y2) {

    float l0 = eq(x1, y1, x2, y2, x, y);
    float l1 = eq(x2, y2, x0, y0, x, y);
    float l2 = eq(x0, y0, x1, y1, x, y);
    float sl0 = eq(x1, y1, x0, y0, x, y);
    float sl1 = eq(x0, y0, x2, y2, x, y);
    float sl2 = eq(x2, y2, x1, y1, x, y);

    if (l0 >= 0 && l1 >= 0 && l2 >= 0) {
        return true;
    }
    if (sl0 >= 0 && sl1 >= 0 && sl2 >= 0) {
        return true;
    }
    return false;
  }

  // Rasterize a triangle.
  void RasterizerImp::rasterize_triangle(float x0, float y0,
    float x1, float y1,
    float x2, float y2,
    Color color) {
    // TODO: Task 1: Implement basic triangle rasterization here, no supersampling
      float minx = floor(min(x0, min(x1, x2)));
      float maxx = ceil(max(x0, max(x1, x2)));
      float miny = floor(min(y0, min(y1, y2)));
      float maxy = ceil(max(y0, max(y1, y2)));
//    for (float x = minx + 0.5; x <= maxx - 0.5; x++) {
//        for (float y = miny + 0.5; y <= maxy - 0.5; y++) {
//            if (insideTriangle(x, y, x0, y0, x1, y1, x2, y2)) {
//                rasterize_point(x, y, color);
//            }
//        }
//    }
    // TODO: Task 2: Update to implement super-sampled rasterization
    int rate = sqrt(sample_rate);
    for (float x = minx; x <= maxx; x++) {
        for (float y = miny; y <= maxy; y++) {
            int sbindex = 0;
            for (int i = 0; i < rate; i++) {
                float sampledx = x + (i+0.5)/rate;
                for (int j = 0; j < rate; j++) {
                    float sampledy = y + (j+0.5)/rate;
                    if (insideTriangle(sampledx, sampledy, x0, y0, x1, y1, x2, y2)) {
                        sample_buffer[(rate * (y * width + x)) + sbindex] = color;
                    }
                    sbindex++;
                }
            }
        }
    }

  }


  void RasterizerImp::rasterize_interpolated_color_triangle(float x0, float y0, Color c0,
    float x1, float y1, Color c1,
    float x2, float y2, Color c2)
  {
    // TODO: Task 4: Rasterize the triangle, calculating barycentric coordinates and using them to interpolate vertex colors across the triangle
          float minx = floor(min(x0, min(x1, x2)));
          float maxx = ceil(max(x0, max(x1, x2)));
          float miny = floor(min(y0, min(y1, y2)));
          float maxy = ceil(max(y0, max(y1, y2)));
          int rate = sqrt(sample_rate);
          Matrix3x3 m(x0, x1, x2, y0, y1, y2, 1, 1, 1);
          Matrix3x3 minv = m.inv();
            
          for (float x = minx; x <= maxx; x++) {
              for (float y = miny; y <= maxy; y++) {
                  int sbindex = 0;
                  for (int i = 0; i < rate; i++) {
                      float sampledx = x + (i+0.5)/rate;
                      for (int j = 0; j < rate; j++) {
                          float sampledy = y + (j+0.5)/rate;
                          if (insideTriangle(sampledx, sampledy, x0, y0, x1, y1, x2, y2)) {
                              Vector3D location(x, y, 1);
                              Vector3D abg = minv * location;
                              float alpha = abg.x;
                              float beta = abg.y;
                              float gamma = 1 - alpha - beta;
                              sample_buffer[(rate * (y * width + x)) + sbindex] =
                                                                alpha * c0 + beta * c1 + gamma * c2;
                          }
                          sbindex++;
                      }
                  }
              }
          }
      


  }


  void RasterizerImp::rasterize_textured_triangle(float x0, float y0, float u0, float v0,
    float x1, float y1, float u1, float v1,
    float x2, float y2, float u2, float v2,
    Texture& tex)
  {
    // TODO: Task 5: Fill in the SampleParams struct and pass it to the tex.sample function.
    // TODO: Task 6: Set the correct barycentric differentials in the SampleParams struct.
    // Hint: You can reuse code from rasterize_triangle/rasterize_interpolated_color_triangle
            float minx = round(min(x0, min(x1, x2)));
            float maxx = round(max(x0, max(x1, x2)));
            float miny = round(min(y0, min(y1, y2)));
            float maxy = round(max(y0, max(y1, y2)));
            int rate = sqrt(sample_rate);
            Matrix3x3 m(x0, x1, x2, y0, y1, y2, 1, 1, 1);
            Matrix3x3 minv = m.inv();
            SampleParams sp;
            for (float x = minx; x <= maxx; x++) {
                for (float y = miny; y <= maxy; y++) {
                    int sbindex = 0;
                    Vector3D location(x, y, 1);
                    Vector3D abg = minv * location;
                    Vector3D abgdx;
                    Vector3D abgdy;
                    abgdx = minv * (location + Vector3D(1, 0, 0));
                    abgdy = minv * (location + Vector3D(0, 1, 0));
                    if (abg.x < 0.0) {
                        abg.x = 0.0;
                    } else if (abg.z < 0.0) {
                        abg.z = 0.0;
                    } else if (abg.z > 1.0) {
                        abg.z = 1.0;
                    } else if (abg.x > 1.0) {
                        abg.x = 1.0;
                    }
                    
                    abg.y = 1.0 - abg.z - abg.x;
                    for (int i = 0; i < rate; i++) {
                        float sampledx = x + (i+0.5)/rate;
                        for (int j = 0; j < rate; j++) {
                            float sampledy = y + (j+0.5)/rate;
                            if (insideTriangle(sampledx, sampledy, x0, y0, x1, y1, x2, y2)) {
                                Vector3D u = Vector3D(u0, u1, u2);
                                Vector3D v = Vector3D(v0, v1, v2);
                                Vector2D dx_uv = Vector2D(dot(abgdx, u), dot(abgdx, v));
                                Vector2D dy_uv = Vector2D(dot(abgdy, u), dot(abgdy, v));
                                Vector2D uv = Vector2D(dot(abg, u), dot(abg, v));
                                
                                sp.p_uv = uv;
                                sp.p_dx_uv = dx_uv;
                                sp.p_dy_uv = dy_uv;
                                
                                Color color;
                                if (lsm == L_ZERO) {
                                    sp.lsm = L_ZERO;
                                    
                                    if (psm == P_NEAREST) {
                                        sp.psm = P_NEAREST;
                                        color = tex.sample_nearest(sp.p_uv, tex.get_level(sp));
                                    } else if (psm == P_LINEAR) {
                                        sp.psm = P_LINEAR;
                                        color = tex.sample_bilinear(sp.p_uv, tex.get_level(sp));
                                    }
                                } else if (lsm == L_NEAREST) {
                                    sp.lsm = L_NEAREST;
                                    
                                    if (psm == P_NEAREST) {
                                        sp.psm = P_NEAREST;
                                    } else if (psm == P_LINEAR) {
                                        sp.psm = P_LINEAR;
                                    }
                                    color = tex.sample(sp);
                                } else if (lsm == L_LINEAR) {
                                    sp.lsm = L_LINEAR;
                                    
                                    if (psm == P_NEAREST) {
                                        sp.psm = P_NEAREST;
                                    } else if (psm == P_LINEAR) {
                                        sp.psm = P_LINEAR;
                                    }
                                    color = tex.sample(sp);
                                }
                                sample_buffer[(rate * (y * width + x)) + sbindex] = color;
                            }
                            sbindex++;
                        }
                    }
                }
            }



  }

  void RasterizerImp::set_sample_rate(unsigned int rate) {
    // TODO: Task 2: You may want to update this function for supersampling support

    this->sample_rate = rate;

    this->sample_buffer.resize(width * height * sample_rate, Color::White);
  }


  void RasterizerImp::set_framebuffer_target(unsigned char* rgb_framebuffer,
    size_t width, size_t height)
  {
    // TODO: Task 2: You may want to update this function for supersampling support

    this->width = width;
    this->height = height;
    this->rgb_framebuffer_target = rgb_framebuffer;

    this->sample_buffer.resize(width * height * sample_rate, Color::White);
  }


  void RasterizerImp::clear_buffers() {
    std::fill(rgb_framebuffer_target, rgb_framebuffer_target + 3 * width * height, 255);
    std::fill(sample_buffer.begin(), sample_buffer.end(), Color::White);
  }


  // This function is called at the end of rasterizing all elements of the
  // SVG file.  If you use a supersample buffer to rasterize SVG elements
  // for antialising, you could use this call to fill the target framebuffer
  // pixels from the supersample buffer data.
  //
  void RasterizerImp::resolve_to_framebuffer() {
    // TODO: Task 2: You will likely want to update this function for supersampling support
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
//        Color col = sample_buffer[y * width + x];
          Color avgColor = Color(0, 0, 0);
          int rate = sqrt(sample_rate);
          int sbindex = 0;
          for (int i = 0; i < rate; i++) {
              for (int j = 0; j < rate; j++) {
                  Color sampleColor = sample_buffer[rate * (y * width + x) + sbindex];
                  avgColor.r += sampleColor.r;
                  avgColor.g += sampleColor.g;
                  avgColor.b += sampleColor.b;
                  sbindex++;
              }
          }
          avgColor.r /= sample_rate;
          avgColor.g /= sample_rate;
          avgColor.b /= sample_rate;
          fill_pixel(x, y, avgColor);
      }
    }

  }

  Rasterizer::~Rasterizer() { }


}// CGL
