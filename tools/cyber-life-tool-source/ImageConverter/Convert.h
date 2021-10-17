#include <string>

int ConvertFile( std::string const& filename, int depth );

#define CONV_ALLOK 0
#define CONV_ERR_READ 1
#define CONV_ERR_MALFORMED_FILE 2
#define CONV_ERR_WRITE 3
#define CONV_ERR_UNKNOWN_FILETYPE 4

