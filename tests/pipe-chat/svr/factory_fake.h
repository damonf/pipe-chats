#ifndef FACTORY_FAKE_H
#define FACTORY_FAKE_H

#include <boost/di.hpp>
#include <boost/di/extension/injections/factory.hpp>

namespace di = boost::di;

template<class T> using MockWithId = std::pair<int, std::unique_ptr<T>>;

namespace pcserver_tests {

    template<class TMock, class TIMock, class... Ts>
    class FactoryFake : public di::extension::ifactory<TIMock, int> {
        public:

            FactoryFake(MockWithId<TMock>&& mock_with_id) {
                init(std::move(mock_with_id));
            }

            FactoryFake(MockWithId<TMock>&& mock_with_id, Ts&&... more) {
                init(std::move(mock_with_id), std::move(more...));
            }

            auto create(int && mock_id) const -> std::unique_ptr<TIMock> override {

                for (auto& mock: mocks_) {
                    if (mock.first == mock_id) {

                        if (mock.second == nullptr) {
                            throw std::logic_error(
                                    std::string{"invalid entry for id: "} +
                                    std::to_string(mock_id)
                                    );
                        }

                        return std::unique_ptr<TMock>(std::move(mock.second));
                    }
                }

                throw std::logic_error(
                        std::string{"no entry for id: "} +
                        std::to_string(mock_id)
                        );
            }

        private:
            void init(MockWithId<TMock>&& mock_with_id, Ts&&... more) {
                mocks_.push_back(std::move(mock_with_id));
                init(std::move(more...));
            }

            void init(MockWithId<TMock>&& mock_with_id) {
                mocks_.push_back(std::move(mock_with_id));
            }

            mutable std::vector<MockWithId<TMock>> mocks_{};
    };

}

#endif // FACTORY_FAKE_H

