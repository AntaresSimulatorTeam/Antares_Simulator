#pragma once

class IntoUTF8ArgsTranslator
{
public:
	IntoUTF8ArgsTranslator(int argc, char**& argv);
	~IntoUTF8ArgsTranslator();
private:
	int argc_ = 0;
	char**& argv_;
};
