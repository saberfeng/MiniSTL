//
// Created by Sabre Sever on 2017/7/25.
//

#ifndef MINISTL_FUNCTION_H
#define MINISTL_FUNCTION_H

namespace mini {

    template<class Pair>
    class Select1st {
        const typename Pair::first_type operator()(const Pair &p) const {
            return p.first;
        }
    };

}

#endif //MINISTL_FUNCTION_H
