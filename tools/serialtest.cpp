#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/export.hpp>

#include <fstream>
#include <iostream>
#define CV 1

#if CV
#include "caosVar.h"
#include "ser/caosVar.h"
#endif

class foo {
    public:
        int i;
        foo() : i(0) {}
        foo(int j) : i(j) {}
    private:
        friend class boost::serialization::access;

        template<class Archive>
            void serialize(Archive &ar, const unsigned int version) {
                ar & BOOST_SERIALIZATION_NVP(i);
            }
};

int main(void) {

    std::ofstream ofs("sertest.dat");
#if CV
    caosVar null, str, intv, floatv;
    null.reset();
    str.setString("Hello, world!");
    intv.setInt(42);
    floatv.setFloat(0.5);
#endif
    {
        const foo v(42);
        boost::archive::text_oarchive oa(ofs);
        oa << v;
#if CV
        oa << (const caosVar &)null << (const caosVar &)str << (const caosVar &)intv << (const caosVar &)floatv;
#endif
    }
    ofs.close();

#if CV
    caosVar nnull, nstr, nintv, nfloatv;
#endif
    {
        foo v;
        std::ifstream ifs("sertest.dat", std::ios::binary);
        boost::archive::text_iarchive ia(ifs);
        ia >> v;
        std::cout << "v.i = " << v.i << std::endl;
#if CV
        ia >> nnull >> nstr >> nintv >> nfloatv;
#endif
    }

#if CV
#define D(x) std::cout << #x << " = " << x.dump() << std::endl
#else
#define D(x) do { } while (0)
#endif
    D(nnull); D(nstr); D(nintv); D(nfloatv);
    return 0;
}
