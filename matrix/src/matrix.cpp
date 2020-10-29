#include "matrix.h"

namespace task {

    double& Row::operator[](size_t col) {
        return this->data[col];
    }

    const double& Row::operator[](size_t col) const {
        return this->data[col];
    }

    size_t Row::size() const {
        return this->size_;
    }

    Row::Row(const Row& copy) {
        this->data = new double[copy.size()];
        this->size_ = copy.size();

        for (size_t i = 0; i < this->size_; ++i) {
            this->data[i] = copy.data[i];
        }
    }

    Row::~Row() {
        delete[] this->data;
    }

    Row::Row() {
        this->data = new double[1];
        this->data[0] = 0.0;
        this->size_ = 1;
    }

    Row::Row(size_t size) {
        this->data = new double[size];

        for (size_t i = 0; i < size; ++i) {
            this->data[i] = 0.0;
        }

        this->size_ = size;
    }

    Row& Row::operator=(const Row& copy) {
        if (&copy == this) {
            return *this;
        }

        delete[] this->data;
        this->data = new double[copy.size()];
        this->size_ = copy.size();

        for (size_t i = 0; i < size_; ++i) {
            this->data[i] = copy.data[i];
        }

        return *this;
    }

    Matrix::Matrix() {
        this->data = new Row[1];
        data[0] = Row();
        data[0][0] = 1.0;
        dim_size = { 1, 1 };
    }

    Matrix::Matrix(size_t rows, size_t cols) {
        this->data = new Row[rows];
        this->dim_size = { rows, cols };

        for (size_t i = 0; i < rows; ++i) {
            this->data[i] = Row(cols);
        }

        for (size_t i = 0; i < std::min(cols, rows); ++i) {
            this->data[i][i] = 1.0;
        }
    }

    Matrix operator*(const double &a, const Matrix &b) {
        Matrix result(b);
        result *= a;
        return result;
    }

    Matrix::Matrix(const Matrix& copy) {
        this->data = new Row[copy.size().first];
        this->dim_size = copy.size();

        for (size_t i = 0; i < this->dim_size.first; ++i) {
            this->data[i] = Row(copy[i]);
        }
    }

    Matrix& Matrix::operator=(const Matrix& a) {
        if (&a == this) {
            return *this;
        }

        delete[] this->data;

        this->data = new Row[a.size().first];
        this->dim_size = a.size();

        for (size_t i = 0; i < this->dim_size.first; ++i) {
            this->data[i] = Row(a[i]);
        }

        return *this;
    }

    Matrix::~Matrix() {
        delete[] this->data;
    }

    double& Matrix::get(size_t row, size_t col) {
        if (row < this->dim_size.first && col < this->dim_size.second) {
            return this->data[row][col];
        } else {
            throw OutOfBoundsException();
        }
    }

    const double& Matrix::get(size_t row, size_t col) const {
        if (row < this->dim_size.first && col < this->dim_size.second) {
            return this->data[row][col];
        } else {
            throw OutOfBoundsException();
        }
    }

    void Matrix::set(size_t row, size_t col, const double& value) {
        if (row < this->dim_size.first && col < this->dim_size.second) {
            this->data[row][col] = value;
        } else {
            throw OutOfBoundsException();
        }
    }

    void Matrix::resize(size_t new_rows, size_t new_cols) {
        Row* new_data = new Row[new_rows];

        for (size_t i = 0; i < new_rows; ++i) {
            new_data[i] = Row(new_cols);
        }

        size_t row_size = std::min(this->dim_size.first, new_rows);
        size_t col_size = std::min(this->dim_size.second, new_cols);

        for (size_t i = 0; i < row_size; ++i) {
            for (size_t j = 0; j < col_size; ++j) {
                new_data[i][j] = this->data[i][j];
            }
        }

        delete[] this->data;
        this->data = new_data;

        this->dim_size.first = new_rows;
        this->dim_size.second = new_cols;
    }

    Row& Matrix::operator[](size_t row) {
        if (row < this->dim_size.first) {
            return this->data[row];
        } else {
            throw OutOfBoundsException();
        }
    }

    const Row& Matrix::operator[](size_t row) const {
        if (row < this->dim_size.first) {
            return this->data[row];
        } else {
            throw OutOfBoundsException();
        }
    }

    Matrix& Matrix::operator+=(const Matrix& a) {
        if (this->size() != a.size()) {
            throw SizeMismatchException();
        }

        for (size_t i = 0; i < this->dim_size.first; ++i) {
            for (size_t j = 0; j < this->dim_size.second; ++j) {
                this->data[i][j] += a[i][j];
            }
        }

        return *this;
    }

    Matrix& Matrix::operator-=(const Matrix& a) {
        if (this->size() != a.size()) {
            throw SizeMismatchException();
        }

        for (size_t i = 0; i < this->dim_size.first; ++i) {
            for (size_t j = 0; j < this->dim_size.second; ++j) {
                this->data[i][j] -= a[i][j];
            }
        }

        return *this;
    }

    Matrix& Matrix::operator*=(const double& number) {
        for (size_t i = 0; i < this->dim_size.first; ++i) {
            for (size_t j = 0; j < this->dim_size.second; ++j) {
                this->data[i][j] *= number;
            }
        }

        return *this;
    }

    Matrix Matrix::operator+(const Matrix& a) const {
        if (this->size() != a.size()) {
            throw SizeMismatchException();
        }

        Matrix result = *this;
        result += a;

        return result;
    }


    Matrix Matrix::operator-(const Matrix& a) const {
        if (this->size() != a.size()) {
            throw SizeMismatchException();
        }

        Matrix result = *this;
        result -= a;

        return result;
    }

    Matrix Matrix::operator*(const Matrix& a) const {
        if (this->size().second != a.size().first) {
            throw SizeMismatchException();
        }

        Matrix result(this->dim_size.first, a.dim_size.second);

        for (size_t i = 0; i < this->dim_size.first; ++i) {
            for (size_t j = 0; j < a.dim_size.second; ++j) {
                result[i][j] = 0.0;

                for (size_t k = 0; k < this->dim_size.second; ++k) {
                    result[i][j] += this->data[i][k] * a[k][j];
                }
            }
        }

        return result;
    }

    Matrix& Matrix::operator*=(const Matrix& a) {
        *this = *this * a;

        return *this;
    }

    Matrix Matrix::operator*(const double& a) const {
        Matrix result(this->dim_size.first, this->dim_size.second);

        for (size_t i = 0; i < this->dim_size.first; ++i) {
            for (size_t j = 0; j < this->dim_size.second; ++j) {
                result[i][j] = this->data[i].data[j] * a;
            }
        }

        return result;
    }

    Matrix Matrix::operator-() const {
        Matrix result(this->dim_size.first, this->dim_size.second);

        for (size_t i = 0; i < this->dim_size.first; ++i) {
            for (size_t j = 0; j < this->dim_size.second; ++j) {
                result[i][j] = -this->data[i][j];
            }
        }

        return result;
    }

    Matrix Matrix::operator+() const {
        return *this;
    }

    double Matrix::det() const {
        if (dim_size.first == dim_size.second) {
            Matrix copy(*this);

            size_t size = dim_size.first;

            double result = 1.0;
            for (size_t i = 0; i < size; ++i) {
                size_t k = i;

                for (size_t j = i + 1; j < size; ++j) {
                    if (fabs(copy[j][i]) > fabs(copy[k][i])) {
                        k = j;
                    }
                }

                if (abs(copy[k][i]) < EPS) {
                    result = 0;
                    break;
                }

                std::swap_ranges(copy[i].data, copy[i].data + size, copy[k].data);

                if (i != k) {
                    result = -result;
                }

                result *= copy[i][i];

                for (size_t j = i + 1; j < size; ++j) {
                    copy[i][j] /= copy[i][i];
                }

                for (size_t j = 0; j < size; ++j) {
                    if (j != i && abs(copy[j][i]) > EPS) {
                        for (size_t p = i + 1; p < size; ++p) {
                            copy[j][p] -= copy[i][p] * copy[j][i];
                        }
                    }
                }
            }

            return result;
        } else {
            throw SizeMismatchException();
        }
    }


    Matrix Matrix::transposed() const {
        Matrix transponsed_matrix(this->dim_size.second, this->dim_size.first);

        for (size_t i = 0; i < this->dim_size.first; ++i) {
            for (size_t j = 0; j < this->dim_size.second; ++j) {
                transponsed_matrix[j][i] = this->data[i][j];
            }
        }

        return transponsed_matrix;
    }

    void Matrix::transpose() {
        Matrix copy = *this;

        *this = copy.transposed();
    }

    double Matrix::trace() const {
        if (this->dim_size.first == this->dim_size.second) {
            double result = 0.0;

            for (size_t i = 0; i < this->dim_size.first; ++i) {
                result += this->data[i][i];
            }

            return result;
        } else {
            throw SizeMismatchException();
        }
    }

    std::vector<double> Matrix::getRow(size_t row) {
        if (row < this->dim_size.first) {

            std::vector<double> result;
            result.reserve(this->dim_size.second);

            for (size_t j = 0; j < this->dim_size.second; ++j) {
                result.push_back(this->data[row][j]);
            }

            return result;
        } else {
            throw OutOfBoundsException();
        }
    }

    std::vector<double> Matrix::getColumn(size_t column) {
        if (column < this->dim_size.second) {
            std::vector<double> result;
            result.reserve(this->dim_size.first);

            for (size_t i = 0; i < this->dim_size.first; ++i) {
                result.push_back(this->data[i][column]);
            }

            return result;
        } else {
            throw OutOfBoundsException();
        }
    }

    bool Matrix::operator==(const Matrix& a) const {
        if (this->size() == a.size()) {
            for (size_t i = 0; i < this->dim_size.first; ++i) {
                for (size_t j = 0; j < this->dim_size.second; ++j) {
                    if (fabs(this->data[i][j] - a[i][j]) >= EPS) {
                        return false;
                    }
                }
            }

            return true;
        } else {
            throw SizeMismatchException();
        }
    }

    bool Matrix::operator!=(const Matrix& a) const {
        return !(*this == a);
    }

    std::pair<size_t, size_t> Matrix::size() const {
        return this->dim_size;
    }

    std::ostream &operator<<(std::ostream &output, const Matrix &matrix) {
        auto[rows, cols] = matrix.size();

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                output << matrix[i][j] << " ";
            }
            output << "\n";
        }

        return output;
    }

    std::istream &operator>>(std::istream &input, Matrix &matrix) {
        size_t rows, cols;
        input >> rows >> cols;

        matrix = Matrix(rows, cols);
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                input >> matrix[i][j];
            }
        }

        return input;
    }
}
