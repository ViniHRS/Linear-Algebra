#include <stdio.h>
#include <math.h>

int main() {
    //Variables
    int m, n, pivot_row;

    //Getting a matrix
    printf("Inform the number of rows: ");
    scanf("%i", &m);
    printf("Inform the number off columns: ");
    scanf("%i", &n);

    double mat[m][n];
    printf("\nInform all your matrix elements:\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("a[%i][%i]: ", i+1, j+1);
            scanf("%lf", &mat[i][j]);
        }
    }
    printf("\n");

    //Putting the matrix in the Reduced Row Echelon Form
    /*
    A princípio, escreverei o programa pensando sempre que o número de linhas da
    matriz informada sempre será menor que o número de colunas, o que faz sentido
    para a maioria dos sistemas que pretendo escalonar. Entretanto, nem sempre isso
    pode ser verdade (posso ter m = n, por exemplo). Por enquanto, utilizarei essa
    hipótese simplificadora para facilitar, mas, no futuro, tentarei arrumar esse
    código pensado que m < n não é necessariamente verdade
    */
    for (pivot_row = 0; pivot_row < m; pivot_row++) {
        //1) Finding a row that has a[r][c] != 0
        int found_r = pivot_row;
        int pivot_column = -1;
        for (int j = 0; j < n; j++) {
            for (int i = pivot_row; i < m; i++) {
                if (mat[i][j] != 0) {
                    found_r = i;
                    pivot_column = j;
                    break;
                }
            }
            if (pivot_column != -1) {
                break;
            }
        }
        //2) Switching the pivot row
        if (found_r != pivot_row) {
            double change[n];
            for (int c = 0; c < n; c++) {
                change[c] = mat[pivot_row][c];
                mat[pivot_row][c] = mat[found_r][c];
                mat[found_r][c] = change[c];
            }
        }
        //3) Setting '1' as the pivot in pivot row
        double divider = mat[pivot_row][pivot_column];
        for (int c = 0; c < n; c++) {
            mat[pivot_row][c] /= divider;
        }
        //4) Subtracting all the elements in the column that are not the pivot
        for (int i = 0; i < m; i++) {
            if (i != pivot_row) {
                double factor_i = mat[i][pivot_column];
                //Making row[i] -> row[i] - factor*pivot_row
                for (int j = 0; j < n; j++) {
                    mat[i][j] -= factor_i*mat[pivot_row][j];
                }
            }
        }
    }

    //Printing the matrix in the Reduced Row Echelon Form
    printf("Matrix in the reduced row echelon form:\n");
    for (int i = 0; i < m; i++) {
        printf("[ ");
        for (int j = 0; j < n; j++) {
            printf("%.2lf ", mat[i][j]);
        }
        printf("]\n");
    }

    return 0;
}