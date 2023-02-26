#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define W 1200
#define H 1200

int main(int argc, char** argv) {
    double start_time = MPI_Wtime();
    double comtime;
    int rank;
    int size;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double real_min = -2.0;
    double img_min = -1.0;
    double real_max = 1.0;
    double img_max = 1.0;
    int maxit= 1000;

    int rowspp = H / size;
    int srow = rowspp * rank;
    int erow = srow + rowspp;

    if (rank == size - 1) {
        erow = H;
    }

    int* row = (int*)malloc(sizeof(int) * W);
    int* data = (int*)malloc(sizeof(int) * W * rowspp);

    for (int y = srow; y < erow; y++) {
        for (int x = 0; x < W; x++) {
            double c_real = real_min + (real_max - real_min) * x / 1200;
            double c_img = img_min + (img_max - img_min) * y / 1200;

            double z_real = 0.0;
            double z_img = 0.0;

            int iterations = 0;
            while (z_real * z_real + z_img * z_img < 4.0 && iterations < maxit) {
                double next_z_real = z_real * z_real - z_img * z_img + c_real;
                double next_z_imag = 2.0 * z_real * z_img + c_img;
                z_real = next_z_real;
                z_img = next_z_imag;
                iterations++;
            }

            if (iterations == maxit) {
                row[x] = 0;
            } else {
                row[x] = iterations % 256;
            }
        }
        int rowi = (y - srow) * W;
        for (int x = 0; x < W; x++) {
            data[rowi + x] = row[x];
        }
    }

    free(row);

    int* dataf = NULL;
    if (rank == 0) {
        dataf = (int*)malloc(sizeof(int) * W * H);
    }
    
	double starttime = MPI_Wtime();
    MPI_Gather(data, W * rowspp, MPI_INT, dataf, W * rowspp, MPI_INT, 0, MPI_COMM_WORLD);
    double endtime = MPI_Wtime();
    comtime = endtime-starttime;

    if (rank == 0) {
        FILE* file = fopen("mandelbrot.pgm", "wb");
        fprintf(file, "P5\n%d %d\n255\n", 1200, 1200);
        for (int i = 0; i < 1200 * 1200; i++) {
            fputc(dataf[i], file);
        }
        fclose(file);
        free(dataf);
    }

    double end_time = MPI_Wtime();
    double time = end_time-start_time;
    printf("Elapsed time: %f seconds\n", time);

    printf("comtime: %f seconds\n", comtime);
    MPI_Finalize();
}

