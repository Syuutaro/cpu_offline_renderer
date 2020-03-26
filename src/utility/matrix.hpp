#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <initializer_list>
#include <cmath>
#include <array>

template <typename T,size_t N>
class Vector{
private:
    T m_c[N];
public:
    //constructor
    Vector():m_c{0}{}
    Vector(std::initializer_list<T> list):m_c{0}{
        if (N == list.size()){
            const T* ite = list.begin();
            for (size_t i = 0;i < N;i++){
                m_c[i] = *ite;
                ++ite;
            }
        }
    }
    Vector(const std::array<T,N>& ary){
        for (size_t i = 0;i < N;i++){
            m_c[i] = ary[i];
        }
    }
    Vector(const Vector<T,N-1>& v,const T& val){
        for (size_t i = 0;i < N-1;i++){
            m_c[i] = v[i];
        }
        m_c[N-1] = val;
    }
    Vector(const Vector<T,N+1>& v){
        for (size_t i = 0;i < N;i++){
            m_c[i] = v[i];
        }
    }

    //substitution
    const Vector<T,N>& operator=(const Vector<T,N-1>& v){
        for (size_t i = 0;i < N-1;i++){
            m_c[i] = v[i];
        }
        m_c[N-1] = 0;
        return *this;
    }
    const Vector<T,N>& operator=(const Vector<T,N+1>& v){
        for (size_t i = 0;i < N;i++){
            m_c[i] = v[i];
        }
        return *this;
    }

    //compare
    bool operator==(const Vector<T,N>& v) const{
        for (size_t i = 0;i < N;i++){
            if (m_c[i] != v[i]){
                return false;
            }
        }
        return true;
    }
    bool operator!=(const Vector<T,N>& v) const{
        for (size_t i = 0;i < N;i++){
            if (m_c[i] != v[i]){
                return true;
            }
        }
        return false;
    }

    //member access
    const T& operator[](size_t i) const{
        return m_c[i];
    }
    T& operator[](size_t i){
        return m_c[i];
    }

    //operation
    //vector_vector
    Vector<T,N> operator+(const Vector<T,N>& v) const{
        Vector<T,N> r;
        for (size_t i = 0;i < N;i++){
            r[i] = m_c[i]+v[i];
        }
        return r;
    }
    Vector<T,N> operator-(const Vector<T,N>& v) const{
        Vector<T,N> r;
        for (size_t i = 0;i < N;i++){
            r[i] = m_c[i]-v[i];
        }
        return r;
    }
    Vector<T,N> operator*(const Vector<T,N>& v) const{
        Vector<T,N> r;
        for (size_t i = 0;i < N;i++){
            r[i] = m_c[i]*v[i];
        }
        return r;
    }
    Vector<T,N> operator/(const Vector<T,N>& v) const{
        Vector<T,N> r;
        for (size_t i = 0;i < N;i++){
            r[i] = m_c[i]/v[i];
        }
        return r;
    }
    const Vector<T,N>& operator+=(const Vector<T,N>& v){
        for (size_t i = 0;i < N;i++){
            m_c[i] += v[i];
        }
        return *this;
    }
    const Vector<T,N>& operator-=(const Vector<T,N>& v){
        for (size_t i = 0;i < N;i++){
            m_c[i] -= v[i];
        }
        return *this;
    }
    const Vector<T,N>& operator*=(const Vector<T,N>& v){
        for (size_t i = 0;i < N;i++){
            m_c[i] *= v[i];
        }
        return *this;
    }
    const Vector<T,N>& operator/=(const Vector<T,N>& v){
        for (size_t i = 0;i < N;i++){
            m_c[i] /= v[i];
        }
        return *this;
    }

    //vector_scalar
    Vector<T,N> operator+(const T& s) const{
        Vector<T,N> r;
        for (size_t i = 0;i < N;i++){
            r[i] = m_c[i]+s;
        }
        return r;
    }
    Vector<T,N> operator-(const T& s) const{
        Vector<T,N> r;
        for (size_t i = 0;i < N;i++){
            r[i] = m_c[i]-s;
        }
        return r;
    }
    Vector<T,N> operator*(const T& s) const{
        Vector<T,N> r;
        for (size_t i = 0;i < N;i++){
            r[i] = m_c[i]*s;
        }
        return r;
    }
    Vector<T,N> operator/(const T& s) const{
        Vector<T,N> r;
        for (size_t i = 0;i < N;i++){
            r[i] = m_c[i]/s;
        }
        return r;
    }
    const Vector<T,N>& operator+=(const T& s){
        for (size_t i = 0;i < N;i++){
            m_c[i] += s;
        }
        return *this;
    }
    const Vector<T,N>& operator-=(const T& s){
        for (size_t i = 0;i < N;i++){
            m_c[i] -= s;
        }
        return *this;
    }
    const Vector<T,N>& operator*=(const T& s){
        for (size_t i = 0;i < N;i++){
            m_c[i] *= s;
        }
        return *this;
    }
    const Vector<T,N>& operator/=(const T& s){
        for (size_t i = 0;i < N;i++){
            m_c[i] /= s;
        }
        return *this;
    }

};

//utility vector function
template <typename T,size_t N>
T dot(const Vector<T,N>& v1,const Vector<T,N>& v2){
    T r = 0;
    for (size_t i = 0;i < N;i++){
        r += v1[i]*v2[i];
    }
    return r;
}

template <typename T,size_t N>
T length(const Vector<T,N>& v){
    return std::sqrt(dot<T,N>(v,v));
}

template <typename T,size_t N>
Vector<T,N> normalize(const Vector<T,N>& v){
    T l = length<T,N>(v);
    if (l != 0){
        return v/l;
    }else{
        return Vector<T,N>();
    }
}

template <typename T>
Vector<T,3> cross(const Vector<T,3>& v1,const Vector<T,3>& v2){
    Vector<T,3> r;
    r[0] = v1[1]*v2[2]-v1[2]*v2[1];
    r[1] = v1[2]*v2[0]-v1[0]*v2[2];
    r[2] = v1[0]*v2[1]-v1[1]*v2[0];
    return r;
}





template <typename T,size_t N>
class Matrix{
private:
    T m_c[N][N];
public:
    //constructor
    Matrix(){
        clear();
    }
    Matrix(std::initializer_list<Vector<T,N>> list){
        if (N == list.size()){
            const Vector<T,N>* ite = list.begin();
            for (size_t i = 0;i < N;i++){
                SetColumn(i,*ite);
                ++ite;
            }
        }else{
            clear();
        }
    }

    //clear component
    void clear(){
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                m_c[i][j] = 0;
            }
        }
    }

    //member access
    void SetComponent(size_t row,size_t column,const T& s){
        m_c[row][column] = s;
    }
    void SetRow(size_t i,const Vector<T,N>& v){
        for (size_t j = 0;j < N;j++){
            m_c[i][j] = v[j];
        }
    }
    void SetColumn(size_t i,const Vector<T,N>& v){
        for (size_t j = 0;j < N;j++){
            m_c[j][i] = v[j];
        }
    }
    const T& GetComponent(size_t row,size_t column) const{
        return m_c[row][column];
    }
    Vector<T,N> GetRow(size_t i) const{
        Vector<T,N> v;
        for (size_t j = 0;j < N;j++){
            v[j] = m_c[i][j];
        }
        return v;
    }
    Vector<T,N> GetColumn(size_t i) const{
        Vector<T,N> v;
        for (size_t j = 0;j < N;j++){
            v[j] = m_c[j][i];
        }
        return v;
    }

    //operation
    //matrix_matrix
    Matrix<T,N> operator+(const Matrix<T,N>& m) const{
        Matrix<T,N> r;
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                r.SetComponent(i,j,m_c[i][j]+m.GetComponent(i,j));
            }
        }
        return r;
    }
    Matrix<T,N> operator-(const Matrix<T,N>& m) const{
        Matrix<T,N> r;
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                r.SetComponent(i,j,m_c[i][j]-m.GetComponent(i,j));
            }
        }
        return r;
    }
    Matrix<T,N> operator*(const Matrix<T,N>& m) const{
        Matrix<T,N> r;
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                r.SetComponent(i,j,dot<T,N>(GetRow(i),m.GetColumn(j)));
            }
        }
        return r;
    }
    const Matrix<T,N>& operator+=(const Matrix<T,N>& m){
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                m_c[i][j] += m.GetComponent(i,j);
            }
        }
        return *this;
    }
    const Matrix<T,N>& operator-=(const Matrix<T,N>& m){
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                m_c[i][j] -= m.GetComponent(i,j);
            }
        }
        return *this;
    }

    //matrix_vector
    Vector<T,N> operator*(const Vector<T,N>& v) const{
        Vector<T,N> r;
        for (size_t i = 0;i < N;i++){
            r[i] = dot(GetRow(i),v);
        }
        return r;
    }

    //Matrix_Scalar
    Matrix<T,N> operator+(const T& s) const{
        Matrix<T,N> r;
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                r.SetComponent(i,j,m_c[i][j]+s);
            }
        }
        return r;
    }
    Matrix<T,N> operator-(const T& s) const{
        Matrix<T,N> r;
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                r.SetComponent(i,j,m_c[i][j]-s);
            }
        }
        return r;
    }
    Matrix<T,N> operator*(const T& s) const{
        Matrix<T,N> r;
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                r.SetComponent(i,j,m_c[i][j]*s);
            }
        }
        return r;
    }
    Matrix<T,N> operator/(const T& s) const{
        Matrix<T,N> r;
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                r.SetComponent(i,j,m_c[i][j]/s);
            }
        }
        return r;
    }
    const Matrix<T,N>& operator+=(const T& s){
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                m_c[i][j] += s;
            }
        }
        return *this;
    }
    const Matrix<T,N>& operator-=(const T& s){
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                m_c[i][j] -= s;
            }
        }
        return *this;
    }
    const Matrix<T,N>& operator*=(const T& s){
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                m_c[i][j] *= s;
            }
        }
        return *this;
    }
    const Matrix<T,N>& operator/=(const T& s){
        for (size_t i = 0;i < N;i++){
            for (size_t j = 0;j < N;j++){
                m_c[i][j] /= s;
            }
        }
        return *this;
    }
};

//utility matrix function
template <typename T,size_t N>
Matrix<T,N> transpose(const Matrix<T,N>& m){
    Matrix<T,N> r;
    for (size_t i = 0;i < N;i++){
        for (size_t j = 0;j < N;j++){
            r.SetComponent(i,j,m.GetComponent(j,i));
        }
    }
    return r;
}

template <typename T,size_t N>
Matrix<T,N-1> remove(const Matrix<T,N>& m,size_t row,size_t column){
    Matrix<T,N-1> r;
    for (size_t i = 0;i < N;i++){
        for (size_t j = 0;j < N;j++){
            if (i < row && j < column){
                r.SetComponent(i,j,m.GetComponent(i,j));
                continue;
            }
            if (i < row && j > column){
                r.SetComponent(i,j-1,m.GetComponent(i,j));
                continue;
            }
            if (i > row && j < column){
                r.SetComponent(i-1,j,m.GetComponent(i,j));
                continue;
            }
            if (i > row && j > column){
                r.SetComponent(i-1,j-1,m.GetComponent(i,j));
                continue;
            }
        }
    }
    return r;
}

template <size_t N>
float determinant(const Matrix<float,N>& m){
    float r = 0;
    for (size_t i = 0;i < N;i++){
        if ((i+0)%2 == 0){
            r += m.GetComponent(i,0)*determinant<N-1>(remove<float,N>(m,i,0));
        }else{
            r -= m.GetComponent(i,0)*determinant<N-1>(remove<float,N>(m,i,0));
        }
    }
    return r;
}

template <>
float determinant<2>(const Matrix<float,2>& m);


template <size_t N>
double determinant(const Matrix<double,N>& m){
    double r = 0;
    for (size_t i = 0;i < N;i++){
        if ((i+0)%2 == 0){
            r += m.GetComponent(i,0)*determinant<N-1>(remove<double,N>(m,i,0));
        }else{
            r -= m.GetComponent(i,0)*determinant<N-1>(remove<double,N>(m,i,0));
        }
    }
    return r;
}


template <>
double determinant<2>(const Matrix<double,2>& m);

template <size_t N>
Vector<float,N> cramer(const Matrix<float,N>& A,const Vector<float,N>& b){
    Vector<float,N> r;
    float det_A = determinant<N>(A);
    if (det_A != 0){
        for (size_t i = 0;i < N;i++){
            Matrix<float,N> A_i = A;
            A_i.SetColumn(i,b);
            r[i] = determinant<N>(A_i)/det_A;
        }
    }
    return r;
}

template <size_t N>
Vector<double,N> cramer(const Matrix<double,N>& A,const Vector<double,N>& b){
    Vector<double,N> r;
    double det_A = determinant<N>(A);
    if (det_A != 0){
        for (size_t i = 0;i < N;i++){
            Matrix<double,N> A_i = A;
            A_i.SetColumn(i,b);
            r[i] = determinant<N>(A_i)/det_A;
        }
    }
    return r;
}


#endif // MATRIX_HPP
