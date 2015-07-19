#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <assert.h>

/**
 * author: Rabindra Harlalka
 *
 * This program prettifies obfuscated JavaScript code, so that it becomes readable.
 */

/**
 * Test output of this program using a JS compiler, such as Rhino as shown in this example:
 * > java -cp d:\js\rhino1.7.6\js.jar org.mozilla.javascript.tools.jsc.Main output.js
 */

int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		std::cerr << "Error: Input file name required as argument" << std::endl;
		return -1;
	}
	
	// open the file
	std::ifstream fin(argv[1]);

	char tab[] = "  ";
	unsigned short indentation = 0;
	
	char k;
	char prev_char = 0;
	std::stringstream last_token;
	
	bool inside_quote = false;
	bool inside_single_quote = false;
	bool inside_pattern = false;
	bool inside_char_class = false;
	bool skip_block = false;

	while ( (k = fin.get()) && k != EOF)
	{
		switch (k)
		{
		case ';':
			std::cout << k;
			if (! inside_quote && ! inside_single_quote && ! inside_pattern)
			{
				std::cout << '\n';
				for (int i = 0; i < indentation; i++) std::cout << tab;
			}
			break;
		case '{':
			if (! inside_quote && ! inside_single_quote && !inside_pattern)
			{
				if (last_token.str() == "return")
				{
					skip_block = true;
				}
				else
				{
					std::cout << '\n';
					for (int i = 0; i < indentation; i++) std::cout << tab;
					indentation++;
				}
			}
			std::cout << k; 
			break;
		case '}':
			if (! inside_quote && ! inside_single_quote && !inside_pattern && !skip_block)
			{
				assert(indentation > 0);	// prevent underflow
				indentation--;
				
				if (prev_char != '{')
				{
					std::cout << '\n';
					for (int i = 0; i < indentation; i++) std::cout << tab;
				}
			}
			std::cout << k;
			break;
		case '"':
			if (! inside_single_quote && !inside_pattern && prev_char != '\\' && !inside_char_class)
				inside_quote = !inside_quote;
			std::cout << k;
			break;
		case '\'':
			if (! inside_quote && !inside_pattern)
				inside_single_quote = !inside_single_quote;
			std::cout << k;
			break;
		case '/':
			if (! inside_single_quote && ! inside_quote && prev_char != '\\' && !inside_char_class)
			{
				if (!inside_pattern)
				{
					if (last_token.str() == "" || last_token.str() == "return")	// distinguish from division operator
					{
						inside_pattern = true;
					}
				}
				else
				{
					inside_pattern = false;
				}
			}
			std::cout << k;
			break;
		case '[':
			if (inside_pattern && prev_char != '\\')
				inside_char_class = true;
			std::cout << k;
			break;
		case ']':
			if (inside_char_class && prev_char != '\\') inside_char_class = false;
			std::cout << k;
			break;
		default:
			if ( (prev_char == '}' || prev_char == '{') && !inside_pattern && !inside_quote && !inside_single_quote && !skip_block)
			{
				std::cout << '\n';
				for (int i = 0; i < indentation; i++) std::cout << tab;
			}
			std::cout << k;
			if (prev_char == '}' && skip_block)
			{
				skip_block = false;
			}
		}
		prev_char = k;
		if (isalnum(k) || isdigit(k) || k == '_')
		{
			last_token << k;
		}
		else if (!isspace(k))
		{
			last_token.str("");
		}

		/// DEBUG
#ifdef DEBUG
		std::cerr << k << '[';
		std::cerr << inside_single_quote;
		std::cerr << inside_quote;
		std::cerr << inside_pattern;
		std::cerr << inside_char_class;
		std::cerr << ']';
		std::cerr << ',' << indentation;
		std::cerr << ',' << last_token.str();
		std::cerr << std::endl;
#endif
	}

	// close the input file
	fin.close();
	return 0;
}
