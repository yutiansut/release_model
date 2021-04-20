#pragma once
namespace hft {
template <typename T, typename TItr>
void mean(const TItr& _begin, const TItr& _end, T& mean) {
    T sum{0};
    for (TItr itr = _begin; itr < _end; ++itr) {
        sum += *itr;
    }
    mean = sum / (_end - _begin);
    return;
}
};  // namespace hft
