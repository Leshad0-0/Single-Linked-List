#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>

#include "single_linked_list.h"

void TestLinkedList() {
    std::cout << "TestLinkedList" << std::endl;
    struct DeletionSpy {
        DeletionSpy() = default;

        explicit DeletionSpy(int &instance_counter) noexcept
                : instance_counter_ptr_(&instance_counter)  //
        {
            OnAddInstance();
        }

        DeletionSpy(const DeletionSpy &other) noexcept
                : instance_counter_ptr_(other.instance_counter_ptr_)  //
        {
            OnAddInstance();
        }

        DeletionSpy &operator=(const DeletionSpy &rhs) noexcept {
            if (this != &rhs) {
                auto rhs_copy(rhs);
                std::swap(instance_counter_ptr_, rhs_copy.instance_counter_ptr_);
            }
            return *this;
        }

        ~DeletionSpy() {
            OnDeleteInstance();
        }

    private:
        void OnAddInstance() noexcept {
            if (instance_counter_ptr_) {
                ++(*instance_counter_ptr_);
            }
        }

        void OnDeleteInstance() noexcept {
            if (instance_counter_ptr_) {
                assert(*instance_counter_ptr_ != 0);
                --(*instance_counter_ptr_);
            }
        }

        int *instance_counter_ptr_ = nullptr;
    };

    // Вспомогательный класс, бросающий исключение после создания N-копии
    struct ThrowOnCopy {
        ThrowOnCopy() = default;

        explicit ThrowOnCopy(int &copy_counter) noexcept
                : countdown_ptr(&copy_counter) {
        }

        ThrowOnCopy(const ThrowOnCopy &other)
                : countdown_ptr(other.countdown_ptr)  //
        {
            if (countdown_ptr) {
                if (*countdown_ptr == 0) {
                    throw std::bad_alloc();
                } else {
                    --(*countdown_ptr);
                }
            }
        }

        // Присваивание элементов этого типа не требуется
        ThrowOnCopy &operator=(const ThrowOnCopy &rhs) = delete;

        // Адрес счётчика обратного отсчёта. Если не равен nullptr, то уменьшается при каждом копировании.
        // Как только обнулится, конструктор копирования выбросит исключение
        int *countdown_ptr = nullptr;
    };

    // Проверка вставки в начало
    {
        SingleLinkedList<int> l;
        assert(l.empty());
        assert(l.size() == 0u);

        l.push_front(0);
        l.push_front(1);
        assert(l.size() == 2);
        assert(!l.empty());

        l.clear();
        assert(l.size() == 0);
        assert(l.empty());
    }

    // Проверка фактического удаления элементов
    {
        int item0_counter = 0;
        int item1_counter = 0;
        int item2_counter = 0;
        {
            SingleLinkedList<DeletionSpy> list;
            list.push_front(DeletionSpy{item0_counter});
            list.push_front(DeletionSpy{item1_counter});
            list.push_front(DeletionSpy{item2_counter});

            assert(item0_counter == 1);
            assert(item1_counter == 1);
            assert(item2_counter == 1);
            list.clear();
            assert(item0_counter == 0);
            assert(item1_counter == 0);
            assert(item2_counter == 0);

            list.push_front(DeletionSpy{item0_counter});
            list.push_front(DeletionSpy{item1_counter});
            list.push_front(DeletionSpy{item2_counter});
            assert(item0_counter == 1);
            assert(item1_counter == 1);
            assert(item2_counter == 1);
        }
        assert(item0_counter == 0);
        assert(item1_counter == 0);
        assert(item2_counter == 0);
    }

    // Проверка обеспечения строгой гарантии безопасности исключений
    {
        bool exception_was_thrown = false;
        // Последовательно уменьшаем счётчик копирований до нуля, пока не будет выброшено исключение
        for (int max_copy_counter = 5; max_copy_counter >= 0; --max_copy_counter) {
            // Создаём непустой список
            SingleLinkedList<ThrowOnCopy> list;
            list.push_front(ThrowOnCopy{});
            try {
                int copy_counter = max_copy_counter;
                list.push_front(ThrowOnCopy(copy_counter));
                // Если метод не выбросил исключение, список должен перейти в новое состояние
                assert(list.size() == 2);
            } catch (const std::bad_alloc &) {
                exception_was_thrown = true;
                // После выбрасывания исключения состояние списка должно остаться прежним
                assert(list.size() == 1);
                break;
            }
        }
        assert(exception_was_thrown);
    }

    // Итерирование по пустому списку
    {
        SingleLinkedList<int> list;
        const auto &const_list = list;

        assert(list.begin() == list.end());
        assert(const_list.begin() == const_list.end());
        assert(list.cbegin() == list.cend());
        assert(list.cbegin() == const_list.begin());
        assert(list.cend() == const_list.end());
    }

    // Итерирование по непустому списку
    {
        SingleLinkedList<int> list;
        const auto &const_list = list;

        list.push_front(1);
        assert(list.size() == 1u);
        assert(!list.empty());

        assert(const_list.begin() != const_list.end());
        assert(const_list.cbegin() != const_list.cend());
        assert(list.begin() != list.end());

        assert(const_list.begin() == const_list.cbegin());

        assert(*list.cbegin() == 1);
        *list.begin() = -1;
        assert(*list.cbegin() == -1);

        const auto old_begin = list.cbegin();
        list.push_front(2);
        assert(list.size() == 2);

        const auto new_begin = list.cbegin();
        assert(new_begin != old_begin);
        // Проверка прединкремента
        {
            auto new_begin_copy(new_begin);
            assert((++(new_begin_copy)) == old_begin);
        }
        // Проверка постинкремента
        {
            auto new_begin_copy(new_begin);
            assert(((new_begin_copy)++) == new_begin);
            assert(new_begin_copy == old_begin);
        }
        // Итератор, указывающий на позицию после последнего элемента, равен итератору end()
        {
            auto old_begin_copy(old_begin);
            assert((++old_begin_copy) == list.end());
        }
    }

    // Преобразование итераторов
    {
        SingleLinkedList<int> list;
        list.push_front(1);
        // Конструирование ConstIterator из Iterator
        SingleLinkedList<int>::ConstIterator const_it(list.begin());
        assert(const_it == list.cbegin());
        assert(*const_it == *list.cbegin());

        SingleLinkedList<int>::ConstIterator const_it1;
        // Присваивание ConstIterator'у значения Iterator
        const_it1 = list.begin();
        assert(const_it1 == const_it);
    }

    // Проверка оператора ->
    {
        using namespace std;
        SingleLinkedList<std::string> string_list;

        string_list.push_front("one"s);
        assert(string_list.cbegin()->length() == 3u);
        string_list.begin()->push_back('!');
        assert(*string_list.begin() == "one!"s);
    }

    // Проверка списков на равенство и неравенство
    {
        SingleLinkedList<int> list_1;
        list_1.push_front(1);
        list_1.push_front(2);

        SingleLinkedList<int> list_2;
        list_2.push_front(1);
        list_2.push_front(2);
        list_2.push_front(3);

        SingleLinkedList<int> list_1_copy;
        list_1_copy.push_front(1);
        list_1_copy.push_front(2);

        SingleLinkedList<int> empty_list;
        SingleLinkedList<int> another_empty_list;

        // Список равен самому себе
        assert(list_1 == list_1);
        assert(empty_list == empty_list);

        // Списки с одинаковым содержимым равны, а с разным - не равны
        assert(list_1 == list_1_copy);
        assert(list_1 != list_2);
        assert(list_2 != list_1);
        assert(empty_list == another_empty_list);
    }

    // Обмен содержимого списков
    {
        SingleLinkedList<int> first;
        first.push_front(1);
        first.push_front(2);

        SingleLinkedList<int> second;
        second.push_front(10);
        second.push_front(11);
        second.push_front(15);

        const auto old_first_begin = first.begin();
        const auto old_second_begin = second.begin();
        const auto old_first_size = first.size();
        const auto old_second_size = second.size();

        first.swap(second);

        assert(second.begin() == old_first_begin);
        assert(first.begin() == old_second_begin);
        assert(second.size() == old_first_size);
        assert(first.size() == old_second_size);

        // Обмен при помощи функции swap
        {
            using std::swap;
            swap(first, second);
            assert(first.begin() == old_first_begin);
            assert(second.begin() == old_second_begin);
            assert(first.size() == old_first_size);
            assert(second.size() == old_second_size);
        }
    }

    // Инициализация списка при помощи std::initializer_list
    {
        SingleLinkedList<int> list{1, 2, 3, 4, 5};
        assert(list.size() == 5);
        assert(!list.empty());
        assert(std::equal(list.begin(), list.end(), std::begin({1, 2, 3, 4, 5})));
    }

    // Лексикографическое сравнение списков
    {
        using IntList = SingleLinkedList<int>;

        assert((IntList{1, 2, 3} < IntList{1, 2, 3, 1}));
        assert((IntList{1, 2, 3} <= IntList{1, 2, 3}));
        assert((IntList{1, 2, 4} > IntList{1, 2, 3}));
        assert((IntList{1, 2, 3} >= IntList{1, 2, 3}));
    }

    // Копирование списков
    {
        const SingleLinkedList<int> empty_list{};
        // Копирование пустого списка
        {
            auto list_copy(empty_list);
            assert(list_copy.empty());
        }

        SingleLinkedList<int> non_empty_list{1, 2, 3, 4};
        // Копирование непустого списка
        {
            auto list_copy(non_empty_list);

            assert(non_empty_list.begin() != list_copy.begin());
            assert(list_copy == non_empty_list);
        }
    }

    // Присваивание списков
    {
        const SingleLinkedList<int> source_list{1, 2, 3, 4};

        SingleLinkedList<int> receiver{5, 4, 3, 2, 1};
        receiver = source_list;
        assert(receiver.begin() != source_list.begin());
        assert(receiver == source_list);
    }

    // Безопасное присваивание списков
    {
        SingleLinkedList<ThrowOnCopy> src_list;
        src_list.push_front(ThrowOnCopy{});
        src_list.push_front(ThrowOnCopy{});
        auto thrower = src_list.begin();
        src_list.push_front(ThrowOnCopy{});

        int copy_counter = 0;  // при первом же копировании будет выброшего исключение
        thrower->countdown_ptr = &copy_counter;

        SingleLinkedList<ThrowOnCopy> dst_list;
        dst_list.push_front(ThrowOnCopy{});
        int dst_counter = 10;
        dst_list.begin()->countdown_ptr = &dst_counter;
        dst_list.push_front(ThrowOnCopy{});

        try {
            dst_list = src_list;
            // Ожидается исключение при присваивании
            assert(false);
        } catch (const std::bad_alloc &) {
            // Проверяем, что состояние списка-приёмника не изменилось
            // при выбрасывании исключений
            assert(dst_list.size() == 2);
            auto it = dst_list.begin();
            assert(it != dst_list.end());
            assert(it->countdown_ptr == nullptr);
            ++it;
            assert(it != dst_list.end());
            assert(it->countdown_ptr == &dst_counter);
            assert(dst_counter == 10);
        } catch (...) {
            // Других типов исключений не ожидается
            assert(false);
        }
    }

    struct DeletionSpy2 {
        ~DeletionSpy2() {
            if (deletion_counter_ptr) {
                ++(*deletion_counter_ptr);
            }
        }

        int *deletion_counter_ptr = nullptr;
    };

    // Проверка pop_front
    {
        SingleLinkedList<int> numbers{3, 14, 15, 92, 6};
        numbers.pop_front();
        assert((numbers == SingleLinkedList<int>{14, 15, 92, 6}));

        SingleLinkedList<DeletionSpy2> list;
        list.push_front(DeletionSpy2{});
        int deletion_counter = 0;
        list.begin()->deletion_counter_ptr = &deletion_counter;
        assert(deletion_counter == 0);
        list.pop_front();
        assert(deletion_counter == 1);
    }

    // Доступ к позиции, предшествующей begin
    {
        SingleLinkedList<int> empty_list;
        const auto &const_empty_list = empty_list;
        assert(empty_list.before_begin() == empty_list.cbefore_begin());
        assert(++empty_list.before_begin() == empty_list.begin());
        assert(++empty_list.cbefore_begin() == const_empty_list.begin());

        SingleLinkedList<int> numbers{1, 2, 3, 4};
        const auto &const_numbers = numbers;
        assert(numbers.before_begin() == numbers.cbefore_begin());
        assert(++numbers.before_begin() == numbers.begin());
        assert(++numbers.cbefore_begin() == const_numbers.begin());
    }

    // Вставка элемента после указанной позиции
    {  // Вставка в пустой список
        {
            SingleLinkedList<int> lst;
            const auto inserted_item_pos = lst.insert_after(lst.before_begin(), 123);
            assert((lst == SingleLinkedList<int>{123}));
            assert(inserted_item_pos == lst.begin());
            assert(*inserted_item_pos == 123);
        }

        // Вставка в непустой список
        {
            SingleLinkedList<int> lst{1, 2, 3};
            auto inserted_item_pos = lst.insert_after(lst.before_begin(), 123);

            assert(inserted_item_pos == lst.begin());
            assert(inserted_item_pos != lst.end());
            assert(*inserted_item_pos == 123);
            assert((lst == SingleLinkedList<int>{123, 1, 2, 3}));

            inserted_item_pos = lst.insert_after(lst.begin(), 555);
            assert(++SingleLinkedList<int>::Iterator(lst.begin()) == inserted_item_pos);
            assert(*inserted_item_pos == 555);
            assert((lst == SingleLinkedList<int>{123, 555, 1, 2, 3}));
        }
    }

    // Удаление элементов после указанной позиции
    {
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto &const_lst = lst;
            const auto item_after_erased = lst.erase_after(const_lst.cbefore_begin());
            assert((lst == SingleLinkedList<int>{2, 3, 4}));
            assert(item_after_erased == lst.begin());
        }
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto item_after_erased = lst.erase_after(lst.cbegin());
            assert((lst == SingleLinkedList<int>{1, 3, 4}));
            assert(item_after_erased == (++lst.begin()));
        }
        {
            SingleLinkedList<int> lst{1, 2, 3, 4};
            const auto item_after_erased = lst.erase_after(++(++lst.cbegin()));
            assert((lst == SingleLinkedList<int>{1, 2, 3}));
            assert(item_after_erased == lst.end());
        }
        {
            SingleLinkedList<DeletionSpy2> list{DeletionSpy2{}, DeletionSpy2{}, DeletionSpy2{}};
            auto after_begin = ++list.begin();
            int deletion_counter = 0;
            after_begin->deletion_counter_ptr = &deletion_counter;
            assert(deletion_counter == 0u);
            list.erase_after(list.cbegin());
            assert(deletion_counter == 1u);
        }
    }
    std::cout << "Done!" << std::endl;
}