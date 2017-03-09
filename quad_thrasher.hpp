#ifndef UUID_A7972692_0ADA_41D3_B90D_F31B297F28CB
#define UUID_A7972692_0ADA_41D3_B90D_F31B297F28CB

#include <random_helper.hpp>
#include <random_quad.hpp>

#include <random>

namespace forensics {
  template <std::size_t max_texture_bytes, std::size_t max_requested_memory_bytes>
  class QuadThrasher final {
  public:
    void draw() {
      auto new_end = std::remove_if(
        begin(quads), end(quads), [&](auto&) { return generator.random_bool(); }
      );
      quads.erase(new_end, end(quads));
      std::vector<std::size_t> sizes{};
      for (auto &quad : quads) { sizes.push_back(quad.size_bytes()); }
      std::size_t bytes_used = std::accumulate(begin(sizes), end(sizes), 0);
      std::size_t delta_bytes = max_requested_memory_bytes / 4;
      std::size_t current_max = generator.random_size(
        max_requested_memory_bytes - delta_bytes,
        max_requested_memory_bytes + delta_bytes
      );
      std::size_t headroom_bytes = current_max - bytes_used;
      std::size_t texture_bytes = generator.random_size(
        max_texture_bytes/2, max_texture_bytes
      );

      while (texture_bytes <= headroom_bytes) {
        std::size_t texels = texture_bytes / 4;
        std::size_t level_zero_size = 3 * (texels / 4);
        std::size_t width = generator.random_size(10, level_zero_size / 10);
        std::size_t height = level_zero_size / width;
        quads.emplace_back(width, height, faker);
        headroom_bytes -= quads.back().size_bytes();
        texture_bytes = generator.random_size(
          max_texture_bytes/2, max_texture_bytes
        );
      }

      for (auto &quad : quads) {
        quad.draw(generator);
      }
    }

  private:
    RandomHelper generator{};
    BufferFaker<max_texture_bytes> faker{generator};
    std::vector<RandomQuad<max_texture_bytes>> quads{};
  };
}

#endif
