
# ifndef SIMPLE_CONVOLUTION_H
# define SIMPLE_CONVOLUTION_H

#include <cstring>
#include <algorithm>

/* 线性卷积核 */
template<typename KerType>
struct KernalMatrix {
    typedef KerType simple_matrix33[3][3];
};


/* 非线性卷积核 */
template<typename ArrType>
struct NonlinearKernalMatrix {
    typedef std::function<ArrType(int, int)> nonlinear_filter;
    typedef std::function<nonlinear_filter(ArrType **)> nonlinear_filter_generator;
};


/* 3x3滤波生成 */
namespace filter_33 {
    
    /* 中值滤波 */
    template<typename ArrType>
    std::function<ArrType(int, int)> median_filter(ArrType **mat)
    {
        return [mat](int x, int y) -> ArrType {
            static ArrType median_arr[10];
            median_arr[0] = mat[x--][y];
            median_arr[1] = mat[x][y--];
            median_arr[2] = mat[x++][y];
            median_arr[3] = mat[x++][y];
            median_arr[4] = mat[x][y++];
            median_arr[5] = mat[x][y++];
            median_arr[6] = mat[x--][y];
            median_arr[7] = mat[x--][y];
            median_arr[8] = mat[x][y];
            std::sort(median_arr, median_arr + 9);
            return median_arr[4];
        };
    }
}


/* 卷积器基类 */
template<typename ArrType>
class convolutor_base
{
protected:
    bool kernaled;

public:
    convolutor_base() {kernaled = false;}
    
    /* 返回卷积核加载状态 */
    bool is_kernal_loaded(){return kernaled;}
    
    /* 卷积函数 */
    virtual void convolute(ArrType **dst, const int row, const int col, ArrType **src) = 0;
};


/* 线性卷积器类 */
template<typename KerType, typename ArrType>
class convolutor_33: public convolutor_base<ArrType>
{
protected:
    typename KernalMatrix<KerType>::simple_matrix33 kernal;
    bool kernaled;
    ArrType **continual_space;
    int mat_row, mat_col;

public:
    convolutor_33(): convolutor_base<ArrType>()
    {
        mat_row = mat_col = 0;
        continual_space = nullptr;
    }
    convolutor_33(const typename KernalMatrix<KerType>::simple_matrix33 loading_kernal): convolutor_base<ArrType>()
    {
        memcpy(kernal, loading_kernal, sizeof(typename KernalMatrix<KerType>::simple_matrix33));
        kernaled = true;
        mat_row = mat_col = 0;
        continual_space = nullptr;
    }

    convolutor_33(const convolutor_33 &con)
    {
        memcpy(kernal, con.kernal, sizeof(typename KernalMatrix<KerType>::simple_matrix33));
        kernaled = con.kernaled;
    }

    convolutor_33(convolutor_33 &&con)
    {
        memcpy(kernal, con.kernal, sizeof(typename KernalMatrix<KerType>::simple_matrix33));
        kernaled = con.kernaled;
        con.kernaled = false;
    }

    /* 加载核矩阵 */
    void load_kernal(const typename KernalMatrix<KerType>::simple_matrix33 loading_kernal)
    {
        memcpy(kernal, loading_kernal, sizeof(typename KernalMatrix<KerType>::simple_matrix33));
        kernaled = true;
    }

    /* 计算卷积 */
    void convolute(ArrType **src, const int row, const int col, ArrType **dst=nullptr) override
    {  
        if (kernaled == false) {
            throw std::runtime_error("kernal is not loaded");
        }
        if (continual_space != nullptr && (row != mat_row || col != mat_col)) {
            for (int i = 0; i < mat_row; i++) {
                delete[] continual_space[i];
            }
            delete[] continual_space;
            continual_space = nullptr;
        }
        if (continual_space == nullptr) {
            // edge protection
            mat_row = row + 2;
            mat_col = col + 2;
            continual_space = new ArrType*[mat_row];
            for (int i = 0; i < mat_row; i++) {
                continual_space[i] = new ArrType[mat_col];
            }
            memset(continual_space[0], 0, sizeof(ArrType) * mat_col);
            memset(continual_space[mat_row - 1], 0, sizeof(ArrType) * mat_col);
        }
        
        int line_size = sizeof(ArrType) * col;
        for (int i = 1; i <= row; i++) {
            continual_space[i][0] = 0;
            memcpy(continual_space[i] + 1, src[i - 1], line_size);
            continual_space[i][col + 1] = 0;
        }
        

        ArrType tmp;
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                src[i][j] = 0;
                for (int ofsx = 0; ofsx < 3; ofsx++) {
                    for (int ofsy = 0; ofsy < 3; ofsy++) {
                        src[i][j] += continual_space[i + ofsx][j + ofsy] * kernal[ofsx][ofsy];
                    }
                }
            }
        }
        
        if (dst != nullptr) {
            if (dst != src) {
                int line_size = sizeof(ArrType) * col;
                for (int i = 0; i < row; i++) {
                    memcpy(dst[i], src[i], line_size);
                }
                for (int i = 0; i < row; i++) {
                    memcpy(src[i], continual_space[i + 1] + 1, line_size);
                }
                for (int i = 0; i < row; i++) {
                    memcpy(continual_space[i + 1] + 1, dst[i], line_size);
                }
            } else {
                for (int i = 0; i < row; i++) {
                    memcpy(continual_space[i + 1] + 1, src[i], line_size);
                }
            }
        }
    }

    /* 返回只读二维矩阵 */
    const ArrType **convolute_space()
    {
        return const_cast<const ArrType **>(continual_space);
    }
};


/* 非线性卷积器类 */
template<typename ArrType>
class convolutor_33_non_linear: public convolutor_base<ArrType>
{
protected:
    typename NonlinearKernalMatrix<ArrType>::nonlinear_filter_generator kernal_generator;
    typename NonlinearKernalMatrix<ArrType>::nonlinear_filter kernal;
    bool kernaled;
    ArrType **continual_space;
    int mat_row, mat_col;

public:

    convolutor_33_non_linear(): convolutor_base<ArrType>()
    {
        mat_row = mat_col = 0;
        continual_space = nullptr;
    }

    convolutor_33_non_linear(
        const typename NonlinearKernalMatrix<ArrType>::nonlinear_filter_generator loading_kernal
    ): convolutor_base<ArrType>() {
        kernal_generator = loading_kernal;
        kernaled = true;
        mat_row = mat_col = 0;
        continual_space = nullptr;
    }

    convolutor_33_non_linear(const convolutor_33_non_linear &con)
    {
        kernal_generator = con.kernal_generator;
        kernal = con.kernal;
        kernaled = con.kernaled;
    }

    convolutor_33_non_linear(convolutor_33_non_linear &&con)
    {
        kernal_generator = con.kernal_generator;
        kernal = con.kernal;
        kernaled = con.kernaled;
        con.kernaled = false;
    }

    
    /* 加载核生成器 */
    void load_kernal(
        const typename NonlinearKernalMatrix<ArrType>::nonlinear_filter_generator loading_kernal
    ) {
        kernal_generator = loading_kernal;
        kernaled = true;
    }

    /* 计算卷积 */
    void convolute(ArrType **src, const int row, const int col, ArrType **dst=nullptr) override
    {  
        if (kernaled == false) {
            throw std::runtime_error("kernal generator is not loaded");
        }
        if (continual_space != nullptr && (row != mat_row || col != mat_col)) {
            for (int i = 0; i < mat_row; i++) {
                delete[] continual_space[i];
            }
            delete[] continual_space;
            continual_space = nullptr;
        }
        if (continual_space == nullptr) {
            // edge protection
            mat_row = row + 2;
            mat_col = col + 2;
            continual_space = new ArrType*[mat_row];
            for (int i = 0; i < mat_row; i++) {
                continual_space[i] = new ArrType[mat_col];
            }
            memset(continual_space[0], 0, sizeof(ArrType) * mat_col);
            memset(continual_space[mat_row - 1], 0, sizeof(ArrType) * mat_col);
        }
        
        int line_size = sizeof(ArrType) * col;
        for (int i = 1; i <= row; i++) {
            continual_space[i][0] = 0;
            memcpy(continual_space[i] + 1, src[i - 1], line_size);
            continual_space[i][col + 1] = 0;
        }
        
        kernal = kernal_generator(continual_space);

        for (int i = 1; i <= row; i++) {
            for (int j = 1; j <= col; j++) {
                src[i - 1][j - 1] = kernal(i, j);
            }
        }
        
        if (dst != nullptr) {
            if (dst == src) {
                for (int i = 0; i < row; i++) {
                    memcpy(continual_space[i + 1] + 1, src[i], line_size);
                }
            } else {
                for (int i = 0; i < row; i++) {
                    memcpy(dst[i], src[i], line_size);
                }
                for (int i = 0; i < row; i++) {
                    memcpy(src[i], continual_space[i + 1] + 1, line_size);
                }
                for (int i = 0; i < row; i++) {
                    memcpy(continual_space[i + 1] + 1, dst[i], line_size);
                }
            }
            
        } else {
            for (int i = 0; i < row; i++) {
                for (int j = 0; j < col; j++) {
                    std::swap(continual_space[i + 1][j + 1], src[i][j]);
                }
            }
        }
    }
    const ArrType **convolute_space()
    {
        return const_cast<const ArrType **>(continual_space);
    }
};


# endif