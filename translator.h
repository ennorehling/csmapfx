#ifndef CSMAP_TRANSLATOR_H
#define CSMAP_TRANSLATOR_H

#include <map>

#include <fx.h>
#include <FXTranslator.h>

class GermanTranslation : public FXTranslator
{
public:
	GermanTranslation(FXApp* app);

	virtual const FXchar* tr(const FXchar* context,const FXchar* message,const FXchar* hint=NULL) const;

private:
	typedef std::map<std::pair<FXString, FXString>, FXString> translation_t;
	translation_t m_trans;
};

#endif //CSMAP_TRANSLATOR_H
