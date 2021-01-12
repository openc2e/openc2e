#include "cmddata.h"
#include "fileformats/caostoken.h"

#include <memory>
#include <string>
#include <vector>

class Dialect;

struct CAOSNode {
	virtual ~CAOSNode() = default;
};

using CAOSNodePtr = std::shared_ptr<CAOSNode>;

struct CAOSCommandNode : CAOSNode {
	CAOSCommandNode(std::string name_, ci_type type_, std::vector<CAOSNodePtr> args_)
		: name(name_), type(type_), args(args_) {}
	std::string name;
	ci_type type;
	std::vector<CAOSNodePtr> args;
	// not needed off the bat, but useful for things like formatters that keep comments
	// size_t start_token;
	// size_t end_token;
	// size_t start_token_in_parent;
	// size_t end_token_in_parent;
};

struct CAOSConditionNode : CAOSNode {
	CAOSConditionNode(std::vector<CAOSNodePtr> args_)
		: args(args_) {}
	std::vector<CAOSNodePtr> args;
};

struct CAOSLiteralValueNode : CAOSNode {
	CAOSLiteralValueNode(caostoken token_)
		: token(token_) {}
	caostoken token;
};

struct CAOSLiteralWordNode : CAOSNode {
	CAOSLiteralWordNode(std::string word_)
		: word(word_) {}
	std::string word;
};

std::vector<CAOSNodePtr> parse(const std::vector<caostoken>& tokens, Dialect* dialect);
