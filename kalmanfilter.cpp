#include "kalmanfilter.h"

#include <assert.h>

Vecd::Vecd(int dim, double v):
    values(dim, v)
{
}

Vecd Vecd::operator+(const Vecd &c2)
{
    assert(values.size() == c2.values.size());
    Vecd result(values.size(), 0);
    std::transform(values.begin(), values.end(), c2.values.begin(), result.values.begin(), std::plus<double>());
    return result;
}

Vecd Vecd::operator-(const Vecd &c2)
{
    assert(values.size() == c2.values.size());
    Vecd result(values.size(), 0);
    std::transform(values.begin(), values.end(), c2.values.begin(), result.values.begin(), std::minus<double>());
    return result;
}

Vecd Vecd::operator*(const Vecd &c2)
{
    assert(values.size() == c2.values.size());
    Vecd result(values.size(), 0);
    std::transform(values.begin(), values.end(), c2.values.begin(), result.values.begin(), std::multiplies<double>());
    return result;
}

Vecd Vecd::operator/(const Vecd &c2)
{
    assert(values.size() == c2.values.size());
    Vecd result(values.size(), 0);
    std::transform(values.begin(), values.end(), c2.values.begin(), result.values.begin(), std::divides<double>());
    return result;
}

KalmanFilter::KalmanFilter(size_t dim):
    m_pstd(0),
    m_cstd(0),
    m_dim(dim),
    X(3, 0),
    X_(3, 0),
    P(3, 1),
    P_(3, 0),
    K(3, 0),
    z(3, 0),
    Q(3, m_pstd),
    R(3, m_cstd),
    ones(3, 1)
{
}

std::vector<double> KalmanFilter::filter(const std::vector<double> &measure)
{
    assert(m_dim == measure.size());
    z.values = measure;
    X_ = X;
    P_ = P + Q;
    K = P_ / (P_ + R);
    X = X_ + K * (z - X_);
    P = (ones - K) * P_;
    return X.values;
}

double KalmanFilter::pstd() const
{
    return m_pstd;
}

void KalmanFilter::setPstd(double pstd)
{
    m_pstd = pstd;
    Q = Vecd(3, m_pstd);
}

double KalmanFilter::cstd() const
{
    return m_cstd;
}

void KalmanFilter::setCstd(double cstd)
{
    m_cstd = cstd;
    R = Vecd(3, m_cstd);
}


