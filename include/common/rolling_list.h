#pragma once

#include <cassert>
#include <string>
#include <cmath>

namespace husky {

    const static int KILO_BYTE = 1 * 1024;
    const static int MEGA_BYTE = 1 * 1024 * 1024;

    struct least_square {
        double a, b;
        double get_y(const double x) {
            return a * x + b;
        }
    };

    template<typename T>
    class rolling_list {
    public:
        ~rolling_list() {
            free(this->_array);
        }

        rolling_list(unsigned int fixed_size, unsigned int pre_allocated_bytes = MEGA_BYTE)
                : _preallocate(pre_allocated_bytes)
        {
            this->_array = (T *) malloc(pre_allocated_bytes);
            memset(_array, 0, pre_allocated_bytes);
            this->_cursor = _array;
            this->fixed_size(fixed_size);
        }

        rolling_list(const rolling_list &other) {
            deep_copy(other);
        }

        rolling_list& operator = (const rolling_list& other) {
            deep_copy(other);
        }

        void fixed_size(uint fixed_size) {
            this->_size = fixed_size;
        }

        void roll_square() {
            _roll_square = true;
        }

        void roll_std() {
            _roll_std = true;
        }

        void roll_min_max() {
            _roll_min_max = true;
        }

        void offer(T value) {
            *_cursor = value;
            do_math();
            cursor_inc();
        }

        void emplace_offer(T &&value) {
            *_cursor = value;
            do_math();
            cursor_inc();
        }

        uint size() {
            return _current_size;
        }

        T front() {
            assert(_current_size > 0);
            return *(_cursor - _current_size);
        }

        T back() {
            assert(_current_size > 0);
            return *(_cursor - 1);
        }

        T& operator[](uint idx) {
            assert(idx < _current_size);
            return *(_cursor - _current_size + idx);
        }

        T sum() {
            return _sum;
        }

        double avg() {
            return _avg;
        }

        T square_sum() {
            return _square_sum;
        }

        double square_avg() {
            return _square_avg;
        }

        /* 需要配置开关 start*/
        double std() {
            return _std;
        }

        T min() {
            return _min;
        }

        T max() {
            return _max;
        }

        T min_idx() {
            return _min_idx;
        }

        T max_idx() {
            return _max_idx;
        }

        /* 需要配置开关 end*/


        std::string tostring() {
            std::string ret;
            for (uint i = 0; i < _current_size; ++i) {
                ret += std::to_string(*(_cursor - _current_size + i));
                if (i < _current_size - 1) {
                    ret += ",";
                }
            }
            return ret;
        }

        void clear() {
            _current_size = 0;
            _max = 0;
            _min = 0;
            _sum = 0;
            _avg = 0;
            _square_sum = 0;
            _square_avg = 0;
            _std = 0;
            if (_array != nullptr) {
                memset(_array, 0, _preallocate);
                _cursor = _array;
            }
        }

        void deep_copy(const rolling_list& other) {
            _preallocate = other._preallocate;
            _size = other._size;
            _current_size = other._current_size;
            _max = other._max;
            _min = other._min;
            _sum = other._sum;
            _avg = other._avg;
            _square_sum = other._square_sum;
            _square_avg = other._square_avg;
            _std = other._std;
            _array = (T *) malloc(_preallocate);
            memcpy(_array, other._array, _preallocate);
            _cursor = other._cursor;
            _roll_square = other._roll_square;
            _roll_std = other._roll_std;
            _roll_min_max = other._roll_min_max;
            _min_idx = other._min_idx;
            _max_idx = other._max_idx;
        }

        double corr( rolling_list<T>& other) {
            if (this->_current_size != other._current_size) return 0.0;
            double sum_sq_x = 0;
            double sum_sq_y = 0;
            double sum_coproduct = 0;
            double mean_x = this->operator[](0);
            double mean_y = other[0];
            double sweep =0;
            double delta_x;
            double delta_y;
            double pop_sd_x;
            double pop_sd_y;
            double cov_x_y;
            for (uint i = 1; i < this->_current_size; ++i) {
                sweep=i/(i+1.0);
                delta_x=this->operator[](i)-mean_x;
                delta_y=other[i]-mean_y;
                sum_sq_x+=delta_x*delta_x*sweep;
                sum_sq_y+=delta_y*delta_y*sweep;
                sum_coproduct+=delta_x*delta_y*sweep;
                mean_x+=delta_x/(i+1.0);
                mean_y+=delta_y/(i+1.0);
            }
            pop_sd_x=sqrt(sum_sq_x/this->_current_size);
            pop_sd_y=sqrt(sum_sq_y/this->_current_size);
            cov_x_y=sum_coproduct/this->_current_size;
            if (pop_sd_x*pop_sd_y == 0) {
                return 0.0;
            }
            return cov_x_y/(pop_sd_x*pop_sd_y);
        }

        double decay_linear() {
            double sum = (1 + _current_size * 1.0) * _current_size / 2;
            double result = 0.0;
            for (uint i = 0; i < _current_size; ++i) {
                result += this->operator[](i) * (i+1.0) / sum;
            }
            return result;
        }

        double var() {
            if (_current_size == 0) return 0;
            double var = 0.0;
            for (uint i = 0; i < _current_size; ++i) {
                var += pow(this->operator[](i) - _avg, 2);
            }
            return var / (_current_size-1);
        }

        double much_bigger() {
            double num1 = 0.0;
            double num2 = 0.0;
            double new_value = this->back();
            for (uint i = 0; i < _current_size; ++i) {
                if (fabs(new_value) >= 3 * fabs(this->operator[](i))) {
                    if (new_value > 0) {
                        num1+=1;
                    } else if (new_value < 0) {
                        num2+=1;
                    }
                }
            }
            if (num1 >= 5 && num1 < 7) {
                return 1;
            } else if (num1 > 7) {
                return 2;
            }
            if (num2 >= 5 && num2 < 7) {
                return -1;
            } else if (num2 > 7) {
                return -2;
            }
            return 0;
        }

        double much_bigger2() {
            double num1 = 0.0;
            double num2 = 0.0;
            double new_value = this->back();
            for (uint i = 0; i <_current_size; ++i) {
                if (new_value > this->operator[](i) && new_value > 0 && this->operator[](i) > 0) {
                    num1+=1;
                } else if (new_value < this->operator[](i) && new_value < 0 && this->operator[](i) < 0) {
                    num2+=1;
                }
            }
            if (num1 >= 5 && num1 < 7) {
                return 1;
            } else if (num1 > 7) {
                return 2;
            }
            if (num2 >= 5 && num2 < 7) {
                return -1;
            } else if (num2 > 7) {
                return -2;
            }
            return 0;
        }

        least_square ls(rolling_list<T>& other) {
            double t1=0, t2=0, t3=0, t4=0;
            for(std::size_t i=0; i<_current_size; ++i) {
                t1 += std::pow(this->operator[](i), 2);
                t2 += this->operator[](i);
                t3 += this->operator[](i) * other[i];
                t4 += other[i];
            }
            return {
                (t3*_current_size - t2*t4) / (t1*_current_size - t2*t2),
                (t1*t4 - t2*t3) / (t1*_current_size - t2*t2)
            };
        }

        least_square leastSquare(rolling_list<T>& other) {
            return ls(other);
        }

    private:
        void do_math() {
            if (_current_size == _size) {
                _sum = _sum - this->front() + *_cursor;
                _avg = (double)_sum / _size;
                if (_roll_std || _roll_square) {
                    _square_sum = _square_sum - pow(this->front(), 2) + pow(*_cursor, 2);
                    _square_avg = (double)_square_sum / _size;
                }
                if (_roll_std) {
                    _std = sqrt(_square_avg - pow(_avg, 2)) * sqrt(_size*1.0/ (_size - 1));
                }
            } else {
                _sum += *_cursor;
                _avg = (double)_sum / (_current_size + 1);
                if (_roll_std || _roll_square) {
                    _square_sum += pow(*_cursor, 2);
                    _square_avg = (double)_square_sum / (_current_size + 1);
                }
                if (_roll_std) {
                    if (_current_size >= 1) {
                        _std = sqrt(_square_avg - pow(_avg, 2)) * sqrt((_current_size+1.0) / _current_size);
                    }
                }
            }
        }

        void cursor_inc() {
            if (_current_size < _size) {
                ++_current_size;
            }
            ++_cursor;
            if (_roll_min_max) {
                _min_idx = 0;
                _max_idx = 0;
                for (uint i = 1; i < _current_size; ++i) {
                    if (this->operator[](_min_idx) > this->operator[](i)) {
                        _min_idx = i;
                    }
                    if (this->operator[](_max_idx) < this->operator[](i)) {
                        _max_idx = i;
                    }
                }
                _min = this->operator[](_min_idx);
                _max = this->operator[](_max_idx);
            }
        }

    public:
        uint _preallocate;
        uint _size = 0;
        uint _current_size = 0;
        T *_array;
        T *_cursor;

        bool _roll_square = false;
        bool _roll_std = false;
        bool _roll_min_max = false;

        T _max;
        T _min;
        T _sum = 0;
        double _avg = 0;
        double _square_sum = 0;
        double _square_avg = 0;
        double _std = 0;

        uint _min_idx;
        uint _max_idx;
    };


    template<typename T>
    class rolling_builder {
    private:
        rolling_list<T> *_ptr;
    public:
        rolling_builder(unsigned int preallocate_bytes = MEGA_BYTE) {
            _ptr = new rolling_list<T>(10, preallocate_bytes);
        }

        rolling_list<T> *create() {
            if (_ptr == nullptr) throw std::runtime_error("rolling list not set yet...");
            return _ptr;
        }

        rolling_builder &fixed_size(uint size) {
            _ptr->fixed_size(size);
            return *this;
        }

        rolling_builder &roll_square() {
            _ptr->roll_square();
            return *this;
        }

        rolling_builder &roll_std() {
            _ptr->roll_std();
            return *this;
        }

        rolling_builder &roll_min_max() {
            _ptr->roll_min_max();
            return *this;
        }
    };
}