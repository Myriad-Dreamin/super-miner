

#include <iostream>
#include "../alter/tools.h"
#include "convolution.h"

int main ()
{
    convolutor_33<double, double> con = convolutor_33<double, double>();
    auto ave = 1.0/9;
    KernalMatrix<double>::simple_matrix33 ker;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            ker[i][j] = ave;
        }
    }


    double **mat=require_two_dimensional_space<double>(4, 4);
    double **mat_dst=require_two_dimensional_space<double>(4, 4);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mat[i][j] = 9;
        }
    }
    con.load_kernal(ker);
    con.convolute(mat, 4, 4, mat_dst);
    PRINT_ARR_2(mat_dst, 4, 4);
    con.convolute(mat_dst, 4, 4, mat_dst);
    PRINT_ARR_2(mat_dst, 4, 4);
    release_two_dimensional_space<double>(&mat, 4);
    release_two_dimensional_space<double>(&mat_dst, 4);
    return 0;
}
