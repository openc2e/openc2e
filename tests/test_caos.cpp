#include "caosScript.h"
#include "caosVM.h"
#include "creaturesImage.h"
#include "imageManager.h"
#include "util.h"
#include "World.h"

#include <gtest/gtest.h>

class Openc2eTestHelper {
public:
    static std::shared_ptr<creaturesImage> createBlnkSprite() {
        shared_ptr<creaturesImage> img(new creaturesImage);
        img->name = "blnk";
        img->imgformat = if_16bit;
        img->m_numframes = 2;
        img->widths = {41, 41};
        img->heights = {18, 18};
        std::vector<unsigned char> pureblack;
        pureblack.resize(2 * 41 * 18, 0);
        img->buffers.push_back(pureblack);
        img->buffers.push_back(pureblack);
        return img;
    }
};

static void run_script(const std::string& dialect, const std::string& s) {
    std::stringstream stream(s);
    caosScript script(dialect, "");
    script.parse(stream);
    
    caosVM vm(nullptr);
    vm.runEntirely(script.installer);
}

TEST(caos, unknown_dialect) {
    ASSERT_ANY_THROW(run_script("unknown dialect", ""));
}
TEST(caos, assert) {
    for (auto dialect : getDialectNames()) {
        run_script(dialect, "dbg: asrt 1 eq 1");
        ASSERT_ANY_THROW(run_script(dialect, "dbg: asrt 1 eq 2"));
        
        run_script(dialect, "dbg: asrf 1 eq 2");
        ASSERT_ANY_THROW(run_script(dialect, "dbg: asrf 1 eq 1"));
        
        ASSERT_ANY_THROW(run_script(dialect, "dbg: fail"));
    }
}
TEST(caos, file) {
    struct auto_data_directories {
        decltype(world.data_directories) data_directories;
        auto_data_directories() {
            data_directories = world.data_directories;
            world.data_directories = { "." };
        }
        ~auto_data_directories() {
            world.data_directories = data_directories;
        }
    } auto_directories;
    run_script("c3", readfile("../tests/file.cos"));
}
TEST(caos, flow) {
    run_script("c3", readfile("../tests/flow.cos"));
}
TEST(caos, ifblocks) {
    run_script("c3", readfile("../tests/ifblocks.cos"));
}
TEST(caos, parse_comment_end) {
    run_script("c3", readfile("../tests/parse-comment-end.cos"));
}
TEST(caos, parsing) {
    run_script("c3", readfile("../tests/parsing.cos"));
}
TEST(caos, simpleagent) {
    auto sprite = Openc2eTestHelper::createBlnkSprite();
    world.gallery->addImage(sprite);
    run_script("c3", readfile("../tests/simpleagent.cos"));
}
TEST(caos, special_lexing) {
    run_script("c1", readfile("../tests/special-lexing.cos"));
    run_script("c2", readfile("../tests/special-lexing.cos"));
}
TEST(caos, strings) {
    run_script("c3", readfile("../tests/strings.cos"));
}
TEST(caos, timeslice) {
    run_script("c1", readfile("../tests/timeslice.cos"));
    run_script("c2", readfile("../tests/timeslice.cos"));
}
TEST(caos, variables) {
    run_script("c3", readfile("../tests/variables.cos"));
}
TEST(caos, vector) {
    run_script("c3", readfile("../tests/vector.cos"));
}