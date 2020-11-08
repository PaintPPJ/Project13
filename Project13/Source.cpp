#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
unsigned long long mod(unsigned long long a, unsigned long long b, unsigned long long modulo)
{

    a %= modulo;
    b %= modulo;


    if (a <= 0xFFFFFFF && b <= 0xFFFFFFF)
    {
        return (a * b) % modulo;
    }
    if (b > a)
    {
        swap(a, b);
    }


    unsigned long long result = 0;
    while (a > 0 && b > 0)
    {

        if (b & 1)
        {
            result += a;
            result %= modulo;
        }


        a <<= 1;
        a %= modulo;
        b >>= 1;
    }
    return result;
}


unsigned long long powermod(unsigned long long base, unsigned long long exponent, unsigned long long modulo)
{
    unsigned long long result = 1;
    while (exponent > 0)
    {
        if (exponent & 1)
            result = mod(result, base, modulo);
        base = mod(base, base, modulo);
        exponent >>= 1;
    }
    return result;
}

// Miller-Rabin-test
bool isPrime(unsigned long long p)
{
    const unsigned int bitmaskPrimes2to31 = (1 << 2) | (1 << 3) | (1 << 5) | (1 << 7) |
        (1 << 11) | (1 << 13) | (1 << 17) | (1 << 19) |
        (1 << 23) | (1 << 29);
    if (p < 31)
        return (bitmaskPrimes2to31 & (1 << p)) != 0;

    if (p % 2 == 0 || p % 3 == 0 || p % 5 == 0 || p % 7 == 0 ||
        p % 11 == 0 || p % 13 == 0 || p % 17 == 0)
        return false;

    if (p < 17 * 19)
        return true;


    const unsigned int Break = 0;
    const unsigned int f[] = { 377687, Break };
    const unsigned int ff[] = { 31, 73, Break };
    const unsigned int fff[] = { 2, 7, 61, Break };
    const unsigned int ffff[] = { 2, 13, 23, 1662803, Break };
    const unsigned int fffff[] = { 2, 325, 9375, 28178, 450775, 9780504, 1795265022, Break };

    const unsigned int* t = fffff;
    if (p < 5329)
        t = f;
    else if (p < 9080191)
        t = ff;
    else if (p < 4759123141ULL)
        t = fff;
    else if (p < 1122004669633ULL)
        t = ffff;

    auto d = p - 1;
    d >>= 1;
    unsigned int shift = 0;
    while ((d & 1) == 0)
    {
        shift++;
        d >>= 1;
    }

    do
    {
        auto x = powermod(*t++, d, p);
        if (x == 1 || x == p - 1)
            continue;

        bool maybePrime = false;
        for (unsigned int r = 0; r < shift; r++)
        {

            x = mod(x, x, p);
            if (x == 1)
                return false;
            if (x == p - 1)
            {
                maybePrime = true;
                break;
            }
        }

        if (!maybePrime)
            return false;
    } while (*t != Break);

    return true;
}



int main()
{
    unsigned int input_number = 50000000;
    cin >> input_number;

    unsigned int count = 0;


    vector<bool> candidate(input_number + 1, true);

    const auto MaxSievePrime = max<unsigned int>(input_number / 50, 10000);
    vector<unsigned int> smallPrimes;
    for (unsigned int p = 3; p <= MaxSievePrime; p++)
        if (isPrime(p))
            smallPrimes.push_back(p);
    const auto r = 2 * MaxSievePrime;

#define PARALLEL
#ifdef  PARALLEL
    unsigned int numCores = 0;
#pragma omp parallel for reduction(+:count) num_threads(numCores) schedule(dynamic, 10000)
#endif
    for (unsigned long long n = 2; n <= input_number; n++)
    {
        if (!candidate[n])
            continue;


        auto p = 2 * n * n - 1;
        if (isPrime(p))
        {
            count++;
            continue;
        }

        if (n < r)
            for (auto s : smallPrimes)
                if (p > s && p % s == 0)
                    for (auto i = n; i <= input_number; i += s)
                        candidate[i] = false;
    }

    cout << count << endl;
    return 0;
}