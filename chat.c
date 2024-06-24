#include <stdio.h>
#include <stdlib.h>

void sort_vertical(int **matrix, int n, int m, int **result_matrix);
void sort_horizontal(int **matrix, int n, int m, int **result_matrix);
void input(int ***matrix, int *n, int *m);
void output(int **matrix, int n, int m);
void deallocate_matrix(int **matrix, int n);

int main()
{
    int **matrix, **result;
    int n, m;

    input(&matrix, &n, &m);

    // Сортировка по вертикали
    result = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        result[i] = (int *)malloc(m * sizeof(int));
    }
    sort_vertical(matrix, n, m, result);
    output(result, n, m);
    printf("\n");

    // Сортировка по горизонтали
    sort_horizontal(matrix, n, m, result);
    output(result, n, m);

    // Освобождение памяти
    deallocate_matrix(matrix, n);
    deallocate_matrix(result, n);

    return 0;
}

void sort_vertical(int **matrix, int n, int m, int **result_matrix)
{
int i, j, k;
for (i = 0; i < n; i++)
{
if (i % 2 == 0)
{
for (j = 0; j < m; j++)
{
for (k = j + 1; k < m; k++)
{
if (matrix[i][j] > matrix[i][k])
{
int temp = matrix[i][j];
matrix[i][j] = matrix[i][k];
matrix[i][k] = temp;
}
}
}
}
else
{
for (j = 0; j < m / 2; j++)
{
int temp = matrix[i][j];
matrix[i][j] = matrix[i][m - 1 - j];
matrix[i][m - 1 - j] = temp;
}
}
}
for (i = 0; i < n; i++)
{
for (j = 0; j < m; j++)
{
result_matrix[i][j] = matrix[i][j];
}
}
}
void sort_horizontal(int **matrix, int n, int m, int **result_matrix)
{
    int i, j, k;

    // Сортировка временной матрицы по горизонтали
    for (i = 0; i < n; i++)
    {
        if (i % 2 == 0)
        {
            for (j = 0; j < m; j++)
            {
                for (k = j + 1; k < m; k++)
                {
                    if (matrix[i][j] > matrix[i][k])
                    {
                        int temp = matrix[i][j];
                        matrix[i][j] = matrix[i][k];
                        matrix[i][k] = temp;
                    }
                }
            }
        }
        else
        {
            for (j = m - 1; j >= 0; j--)
            {
                for (k = j - 1; k >= 0; k--)
                {
                    if (matrix[i][j] > matrix[i][k])
                    {
                        int temp = matrix[i][j];
                        matrix[i][j] = matrix[i][k];
                        matrix[i][k] = temp;
                    }
                }
            }
        }
    }

    // Копирование отсортированной матрицы в результат
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            result_matrix[i][j] = matrix[i][j];
        }
    }

}

void input(int ***matrix, int *n, int *m)
{
    int i, j;

    printf("Enter the number of rows: ");
    scanf("%d", n);

    printf("Enter the number of columns: ");
    scanf("%d", m);

    // Выделение памяти для матрицы
    *matrix = (int **)malloc(*n * sizeof(int *));
    for (i = 0; i < *n; i++)
    {
        (*matrix)[i] = (int *)malloc(*m * sizeof(int));
    }

    printf("Enter the matrix elements:\n");
    for (i = 0; i < *n; i++)
    {
        for (j = 0; j < *m; j++)
        {
            scanf("%d", &((*matrix)[i][j]));
        }
    }
}

void output(int **matrix, int n, int m)
{
    int i, j;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            printf("%d", matrix[i][j]);
            if (j != m - 1)
            {
                printf(" ");
            }
        }
        printf("\n");
    }
}

void deallocate_matrix(int **matrix, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}