#ifndef KALMANFILTER_H
#define KALMANFILTER_H

#include <vector>
#include <algorithm>

class Vecd
{
public:
    Vecd(int dim, double v = 0);
    Vecd operator+(const Vecd &c2);
    Vecd operator-(const Vecd &c2);
    Vecd operator*(const Vecd &c2);
    Vecd operator/(const Vecd &c2);
    std::vector<double> values;
};

class KalmanFilter
{
public:
    explicit KalmanFilter(size_t dim);
    std::vector<double> filter(const std::vector<double> &measure);
    double pstd() const;
    void setPstd(double pstd);
    double cstd() const;
    void setCstd(double cstd);

private:
    double m_pstd;
    double m_cstd;
    size_t m_dim;
    Vecd X;
    Vecd X_;
    Vecd P;
    Vecd P_;
    Vecd K;
    Vecd z;
    Vecd Q;
    Vecd R;
    Vecd ones;
};

#endif // KALMANFILTER_H
