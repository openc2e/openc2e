#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include "serialization.h"
#include "caosScript.h"
#include "bytecode.h"
#include <sstream>

#include <fstream>
#include <iostream>
#ifndef CV
#define CV 1
#endif

#if CV
#include "caosVar.h"
#endif

using namespace std;

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

std::string testString("inst doif 0 eq 0 outv 42 else outs \"Basic mathematics have failed us :(\\n\" endi gsub foobie stop subr foobie dbg: outs \"teh foobie\\n\" retn rscr");

int main(void) {
    registerDelegates();

    std::ofstream ofs("sertest.dat");
#if CV
    caosVar null, str, intv, floatv;
    null.reset();
    str.setString("Hello, world!");
    intv.setInt(42);
    floatv.setFloat(0.5);
#endif
    istringstream iss(testString);
    caosScript iscr("c3", "test");
    iscr.parse(iss);
    std::cout << iscr.installer->dump();
    {
        const foo v(42);
        boost::archive::text_oarchive oa(ofs);
        oa << v;
#if CV
        oa << (const caosVar &)null << (const caosVar &)str << (const caosVar &)intv << (const caosVar &)floatv;
#endif
        oa << (const caosScript &)iscr;
    }
    ofs.close();

#if CV
    caosVar nnull, nstr, nintv, nfloatv;
#endif
    caosScript script;
    {
        foo v;
        std::ifstream ifs("sertest.dat", std::ios::binary);
        boost::archive::text_iarchive ia(ifs);
        ia >> v;
        std::cout << "v.i = " << v.i << std::endl;
#if CV
        ia >> nnull >> nstr >> nintv >> nfloatv;
#endif
        ia >> script;
    }
    std::cout << script.installer->dump();

#if CV
#define D(x) std::cout << #x << " = " << x.dump() << std::endl
#else
#define D(x) do { } while (0)
#endif
    D(nnull); D(nstr); D(nintv); D(nfloatv);
    return 0;
}
