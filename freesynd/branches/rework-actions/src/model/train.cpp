#include "train.h"

Train::Train() {
}

Train::~Train() {
    for (std::list < TrainElement * >::iterator it = elements_.begin();
         it != elements_.end(); it++) {
        TrainElement *elem = *it;
        delete elem;
    }
    elements_.clear();
}

bool Train::animate(int elapsed) {
    return false;
}

TrainElement::TrainElement() : Vehicle(-1, false) {
//TrainElement::TrainElement() : Vehicle(-1) {
}

TrainElement::~TrainElement() {
}