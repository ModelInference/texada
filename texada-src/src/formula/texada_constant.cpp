/* texada_constant.h
 * Created On: December 20, 2021
 * Author: cjbanks
 */

#include "texada_constant.h"

namespace texada {
    namespace ltl {
        constant constant::true_instance_(constant::True);
        constant constant::false_instance_(constant::False);
        constant constant::empty_word_instance_(constant::EmptyWord);



        std::string
        constant::dump() const
        {
            switch (val())
            {
                case True:
                    return "constant(1)";
                case False:
                    return "constant(0)";
                case EmptyWord:
                    return "constant(e)";
            }
        }

        void
        constant::accept(visitor& v) const
        {
            v.visit(this);
        }

        const char*
        constant::val_name() const
        {
            switch (val_)
            {
                case True:
                    return "1";
                case False:
                    return "0";
                case EmptyWord:
                    return "[*0]";
            }
        }
    }
    }