/// GraphManager.h

#include "Manager.h"

class GraphManager : public Manager {
    public: 
        GraphManager() : Manager() {}
        bool initialize() override;
        void cleanup() override;
    private:
        void _threadFunction() override;
};