#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include <thread>
#include <vector>
#include <algorithm>
#include <cstring>

const std::string INCORRECT_INPUT = "Error: Incorrect input";
const std::string INCORRECT_SIZE = "Error: Incorrect matrix size";
const std::string INCORRECT_PATH = "Error: File cannot be opened";
const std::string INCORRECT_ARGUMENT = "Error: The argument is not valid";
const std::string HELP_MESSAGE = "-p enables multithreading. The program will create as many threads as there are "
                                 "cores in your hardware. Without this argument, it uses one thread by default.\n"
                                 "-f value switches the program to read from a file. "
                                 "Value must contain the path to the file.\n"
                                 "-g generates random matrices.\n"
                                 "-o prints all matrices.";

const int INCORRECT_INPUT_CODE = 100;
const int INCORRECT_SIZE_CODE = 110;
const int INCORRECT_PATH_CODE = 120;
const int INCORRECT_ARGUMENT_CODE = 130;

const int RANDOM_DOUBLE_A = 1;
const int RANDOM_DOUBLE_B = 10;
const int RANDOM_SIZE_A = 1;
const int RANDOM_SIZE_B = 5;

const std::string MATRIX_NAME1 = "A";
const std::string MATRIX_NAME2 = "B";
const std::string MATRIX_NAME3 = "C";

std::random_device random_device;
std::default_random_engine random_engine(random_device());

enum Input { terminal, file, generate };

void print_error(int code, const std::string &info){
    std::cerr << info << std::endl;
    exit(code);
}

void output_matrix(std::ostream &out, const std::vector<double> &matrix,
                   size_t m, size_t n, const std::string &name){
    std::cout << "Matrix " << name << ":\n";
    for(size_t row = 0; row < m; row++){
        for(size_t column = 0; column < n; column++)
            out << matrix[row * n + column] << " ";
        out << std::endl;
    }
}

template <typename TimePoint> std::chrono::milliseconds to_ms(TimePoint tp) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp);
}

double get_random_double(int a, int b){
    std::uniform_real_distribution<double> uniform_dist(a, b);
    double random_double = uniform_dist(random_engine);
    return random_double;
}

size_t get_random_size(int a, int b){
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(a,b); // distribution in range [a, b]

    return dist6(rng);
}

std::vector<double> get_random_matrix(size_t m, size_t n){
    std::vector<double> matrix(n * m);
    for(size_t i = 0; i < n*m; i++)
        matrix[i] = get_random_double(RANDOM_DOUBLE_A, RANDOM_DOUBLE_B);

    return matrix;
}

void thread_generate_matrix(std::vector<double> &matrix_A, std::vector<double> &matrix_B,
                 size_t begin1, size_t end1, size_t begin2, size_t end2) {
    for(size_t i = begin1; i < end1; i++)
        matrix_A[i] = get_random_double(RANDOM_DOUBLE_A, RANDOM_DOUBLE_B);
    for(size_t i = begin2; i < end2; i++)
        matrix_B[i] = get_random_double(RANDOM_DOUBLE_A, RANDOM_DOUBLE_B);
}

void thread_multiplication(const std::vector<double> &A, const std::vector<double> &B, std::vector<double> &C,
                           size_t begin, size_t end, size_t n1, size_t m2, size_t n2){
    for (size_t row = begin; row < end; row++)
        for (size_t col = 0; col < n2; col++)
            for (size_t inner = 0; inner < m2; inner++)
                C[row * n2 + col] += A[row * n1 + inner] * B[inner * n2 + col];
}

std::vector<double> matrix_multiplication_parallel(const std::vector<double> &A,
                                                   const std::vector<double> &B,
                                                   size_t m1, size_t n1, size_t m2, size_t n2){
    std::vector<double> C(m1*n2, 0);
    size_t row = m1;
    size_t num_threads = std::thread::hardware_concurrency();
    size_t chunk_size = 1 + row / num_threads;
    std::vector<std::thread> threads;
    for (size_t thread_id = 0; thread_id < num_threads; thread_id++) {
        size_t begin = thread_id * chunk_size;
        size_t end = (thread_id + 1) * chunk_size;
        if (end > row) end = row;
        threads.emplace_back(thread_multiplication, std::ref(A), std::ref(B), std::ref(C),
                             begin,end, n1, m2, n2);
    }
    for (auto &t : threads) t.join();
    return C;
}

std::vector<double> matrix_multiplication(const std::vector<double> &A, const std::vector<double> &B,
                                          size_t m1, size_t n1, size_t m2, size_t n2){
    std::vector<double> C(m1*n2, 0);

    for (size_t row = 0; row < m1; row++)
        for (size_t col = 0; col < n2; col++)
            for (size_t k = 0; k < m2; k++)
                C[row * n2 + col] += A[row * n1 + k] * B[k * n2 + col];
    return C;
}

std::vector<double> get_matrix(size_t m, size_t n, std::istream &stream){
    std::vector<double> matrix(n * m);
    for(size_t row = 0; row < m; row++)
        for(size_t column = 0; column < n; column++){
            stream >> matrix[n * row + column];
            if(!stream) print_error(INCORRECT_INPUT_CODE, INCORRECT_INPUT);
        }

    return  matrix;
}

std::pair<size_t, size_t> get_size_matrix(std::istream &stream){
    int m, n;
    stream >> m >> n;
    if(!std::cin)
        print_error(INCORRECT_INPUT_CODE, INCORRECT_INPUT);
    if(m < 1 || n < 1)
        print_error(INCORRECT_SIZE_CODE, INCORRECT_SIZE);

    return {m, n};
}

void get_random_matrix_parallel(std::vector<double> &matrix_A, std::vector<double> &matrix_B){
    size_t size_A = matrix_A.size();
    size_t size_B = matrix_B.size();
    size_t num_threads = std::thread::hardware_concurrency();
    size_t chunk_size_A = 1 + size_A / num_threads;
    size_t chunk_size_B = 1 + size_B / num_threads;
    std::vector<std::thread> threads;
    for (size_t thread_id = 0; thread_id < num_threads; thread_id++) {
        size_t begin_A = thread_id * chunk_size_A;
        size_t begin_B = thread_id * chunk_size_B;
        size_t end_A = (thread_id + 1) * chunk_size_A;
        size_t end_B = (thread_id + 1) * chunk_size_B;
        if (end_A > size_A) end_A = size_A;
        if (end_B > size_B) end_B = size_B;
        threads.emplace_back(thread_generate_matrix, std::ref(matrix_A), std::ref(matrix_B),
                             begin_A, end_A, begin_B, end_B);
    }
    for (auto &t : threads) t.join();
}

int main(int argc, char** argv) {
    auto start = std::chrono::high_resolution_clock::now();
    std::string path;
    Input input = terminal;
    std::ifstream file_stream;
    bool parallel_mode = false;
    bool output_matrices = false;
    size_t m1 = 0, m2 = 0, n1 = 0, n2 = 0;
    std::vector<double> matrix_A, matrix_B, matrix_C;

    for(int i = 1; i < argc; i++){
        if(std::string(argv[i]) == "--help"){
            std::cout << HELP_MESSAGE << std::endl;
            exit(0);
        } else if(std::string(argv[i]) == "-g"){
            input = generate;
            std::cout << "**Matrices will be generated**\n";
        } else if(std::string(argv[i]) == "-f"){
            input = file;
            path = argv[++i];
            std::cout << "**Input from file " << path << "**\n";
        } else if(std::string(argv[i]) == "-p"){
            parallel_mode = true;
            std::cout << "**Parallel mode**\n";
        } else if(std::string(argv[i]) == "-o"){
            output_matrices = true;
            std::cout << "**Matrices will be printed**\n";
        } else print_error(INCORRECT_ARGUMENT_CODE, INCORRECT_ARGUMENT);
    }

    switch(input){
        case terminal:
            std::tie(m1, n1) = get_size_matrix(std::cin); // input size of the first matrix
            matrix_A = get_matrix(m1, n1, std::cin); // get first matrix

            std::tie(m2, n2) = get_size_matrix(std::cin); // input size of the second matrix
            matrix_B = get_matrix(m2, n2, std::cin); // get second matrix
            break;
        case file:
            file_stream.open(path);
            if(!file_stream.is_open())
                print_error(INCORRECT_PATH_CODE, INCORRECT_PATH);

            std::tie(m1, n1) = get_size_matrix(file_stream); // input size of the first matrix
            matrix_A = get_matrix(m1, n1, file_stream); // get first matrix

            std::tie(m2, n2) = get_size_matrix(file_stream); // input size of the second matrix
            matrix_B = get_matrix(m2, n2, file_stream); // get second matrix
            file_stream.close();
            break;
        case generate:
            m1 = get_random_size(RANDOM_SIZE_A, RANDOM_SIZE_B);
            n1 = get_random_size(RANDOM_SIZE_A, RANDOM_SIZE_B);
            m2 = n1;
            n2 = get_random_size(RANDOM_SIZE_A, RANDOM_SIZE_B);
            if(parallel_mode){
                matrix_A.resize(m1 * n1);
                matrix_B.resize(m2 * n2);
                get_random_matrix_parallel(matrix_A, matrix_B);
            }else{
                matrix_A = get_random_matrix(m1, n1);
                matrix_B = get_random_matrix(m2, n2);
            }
            if(output_matrices){
                output_matrix(std::cout, matrix_A, m1, n1, MATRIX_NAME1);
                output_matrix(std::cout, matrix_B, m2, n2, MATRIX_NAME2);
            }
            break;
    }

    if(parallel_mode)
        matrix_C = matrix_multiplication_parallel(matrix_A, matrix_B, m1, n1, m2, n2);
    else
        matrix_C = matrix_multiplication(matrix_A, matrix_B, m1, n1, m2, n2);
    auto end = std::chrono::high_resolution_clock::now();
    if(output_matrices)
        output_matrix(std::cout, matrix_C, m1, n2, MATRIX_NAME3);
    std::cout << "Needed " << to_ms(end - start).count() << " ms to finish.\n";
    return 0;
}