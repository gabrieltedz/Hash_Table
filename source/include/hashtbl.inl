#include "hashtbl.h"

/*!
 * @file hashtbl.h
 * @brief Definition of the HashTbl class.
 *
 * This file contains the declaration of the HashTbl class, which represents a hash table.
 * Authors: Gabriel Victor and Thiago Raquel.
 */

namespace ac {
    /*!
     * @brief Constructor that initializes the table with a specified size.
     *
     * @param sz Size of the table.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( size_type sz )
    {
        m_size = 0; m_size = sz;
        // Checks if the size chosen by the user is prime, if not, finds the next prime.
        if (!(isPrime(m_size)))
            m_size = nextPrime(sz);

        // Number of data is zero.
        //m_count = 0;
        

        // Initializes the hash with the appropriate and prime size.
        m_table = new std::forward_list<entry_type>[m_size];
    }

    /*!
     * @brief Constructor that initializes the table based on another existing table.
     *
     * @param source The table from which to copy the elements.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::HashTbl(const HashTbl &source)
    {
        // Function to initialize the constructor, the same one used with the operator =
        initialize_hash(source);
        m_count = source.m_count;
        m_size = source.m_size;
    }

    /*!
     * @brief Constructor that initializes the table based on an initializer list.
     *
     * @param ilist Initialization list containing key-value pairs.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::HashTbl(const std::initializer_list<entry_type> &ilist)
    {
        // Function to initialize the constructor, the same one used with the operator =
        initialize_hash_ilist(ilist);
        m_count =  ilist.size();
    }

    /*!
     * @brief Assignment operator that replaces the table's elements with those of another table.
     *
     * @param clone The table to be copied.
     * @return Reference to the table after the copy.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    HashTbl<KeyType, DataType, KeyHash, KeyEqual> &
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator=(const HashTbl &clone)
    {
        // Function to initialize the hash with the assignment operator =, the same one used with the constructor
        initialize_hash(clone);
        m_count = clone.m_count;
        return *this;
    }

    /*!
     * @brief Assignment operator that replaces the table's elements with those of an initializer list.
     *
     * @param ilist Initialization list containing key-value pairs.
     * @return Reference to the table after the copy.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    HashTbl<KeyType, DataType, KeyHash, KeyEqual> &
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator=(const std::initializer_list<entry_type> &ilist)
    {
        // Function to initialize the hash with the assignment operator =, the same one used with the constructor
        initialize_hash_ilist(ilist);
        m_count =  ilist.size();
        return *this;
    }

    /*!
     * @brief Destructor that frees the memory allocated by the table.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::~HashTbl()
    {
        // Delete the hash.
        delete[] m_table;
    }

    /*!
     * @brief Inserts a new key-value pair into the table.
     *
     * @param key_ The key to be inserted.
     * @param new_data_ The data associated with the key.
     * @return true if the insertion was successful, false if the key already exists.
     *
     * This function inserts a new element into the hash table. It calculates the hash index for the key
     * using the hash function and then checks if the key already exists in the corresponding linked list.
     * If the key does not exist, a new key-value pair is inserted at the beginning of the list.
     * If the load on the table exceeds the specified load factor, the function calls rehash to
     * reorganize the table and reduce the load.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::insert(const KeyType &key_, const DataType &new_data_)
    {
        // Calculate the hash index
        auto hash_index = KeyHash()(key_) % m_size;

        // Iterate over the linked list at the hash position
        auto &hash_list = m_table[hash_index];

        // Check if the key already exists in the list
        auto it = std::find_if(hash_list.begin(), hash_list.end(),
                            [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });

        if (it == hash_list.end()) {
            // The key does not exist, add a new element to the list
            //hash_list.emplace_front(key_, new_data_);
            hash_list.push_front(HashEntry(key_, new_data_));
           // std::cout << new_data_ << "\n";
            m_count++;

            if (m_count / m_size > m_factor_load) {
                rehash();
            }
            return true; // Successful insertion
        } 
        else {
            // The key already exists, update the data
            it->m_data = new_data_;
            return false; // Unsuccessful insertion
        }
    }


    /*!
 * @brief Clears all elements from the hash table.
 *
 * This function deletes the nodes from the linked lists at each index.
 */
template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::clear()
{
    // Delete nodes from the linked lists at each index
    for (size_t i{0}; i < m_size; ++i) {
        m_table[i].clear();
    }

    m_count = 0;
}

    /*!
     * @brief Checks if the hash table is empty.
     *
     * @return true if the table is empty, false otherwise.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::empty() const
    {
        return m_count == 0;
    }

    /*!
     * @brief Retrieves the data associated with a given key.
     *
     * @param key_ The key to search for.
     * @param data_item_ The variable to store the retrieved data.
     * @return true if the retrieval was successful, false if the key does not exist.
     *
     * This function iterates over the linked list at the hash position and checks if the key already
     * exists in the list. If the key exists, the associated data is stored in the provided variable.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::retrieve(const KeyType &key_, DataType &data_item_) const
    {
        // Iterate over the linked list at the hash position
        size_t hash_value = KeyHash()(key_) % m_size;
        auto &hash_list = m_table[hash_value];

        // Check if the key already exists in the list
        auto it = std::find_if(hash_list.begin(), hash_list.end(),
                            [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });

        if (it != hash_list.end()) {
            // The key exists, store the associated data in the variable
            data_item_ = it->m_data;
            return true; // Successful retrieval
        }

        return false; // Key not found
    }

    /*!
     * @brief Performs the rehashing process, increasing the size of the hash table and rearranging the elements.
     *
     * Rehashing is necessary when the load factor of the hash table reaches a limit, ensuring proper performance.
     * In this process, the size of the hash table is increased, and all elements are redistributed to the new positions.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::rehash(void)
    {
        // Store the current size of the table
        auto aux = m_size;

        // Check if twice the current size is a prime number
        if (!(isPrime(m_size * 2)))
            m_size = nextPrime(m_size * 2);

        // Create a new hash table with the new size
        std::forward_list<entry_type> *m_table2 = new std::forward_list<entry_type>[m_size];

        // Move elements from the old table to the new table
        for (size_t i = 0; i < aux; i++)
        {
            for (const auto &entry : m_table[i])
            {
                // Calculate the new hash value for the key
                size_t hash_value = KeyHash()(entry.m_key) % m_size;

                // Add the element to the new position in the new table
                m_table2[hash_value].emplace_front(entry.m_key, entry.m_data);
            }
        }

        // Clear the old table
        delete [] m_table;

        // Update the table to point to the new table
        m_table = m_table2;
    }

    /*!
     * @brief Removes an element with the provided key from the hash table.
     *
     * This function searches for an element in the hash table with the specified key and removes it if found.
     *
     * @param key_ The key of the element to be removed.
     * @return true if the removal is successful, false if the key is not found.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::erase(const KeyType &key_)
    {
        // Calculate the hash value for the key
        size_t hash_value = KeyHash()(key_) % m_size;

        // Get the linked list corresponding to the hash position
        auto &hash_list = m_table[hash_value];

        // Search for the element with the provided key in the list
        auto it = std::find_if(hash_list.begin(), hash_list.end(),
                                [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });

        // Check if any element was removed
        if (it != hash_list.end()) {
            // Remove the removed elements from the list
            hash_list.remove_if([&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });
            m_count--; // Update the element count in the table
            return true;
        }

        // The key was not found in the hash table
        return false;
    }

    /*!
     * @brief Returns the number of elements in the hash table.
     *
     * @param key_ The key to count.
     * @return The number of occurrences of the key in the hash table.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    typename HashTbl<KeyType, DataType, KeyHash, KeyEqual>::size_type
    HashTbl<KeyType, DataType, KeyHash, KeyEqual>::count(const KeyType &key_) const
    {
        // Calculate the hash value for the key
        size_t hash_value = KeyHash()(key_) % m_size;

        // Get the linked list corresponding to the hash position
        auto &hash_list = m_table[hash_value];

        size_type count{0};

        for (auto const& entry : hash_list){
            count++;
        }

        return count;
    }

    /*!
     * @brief Retrieves the value associated with the specified key.
     *
     * This function retrieves the value associated with the specified key.
     * If the key is found in the hash table, the corresponding value is returned.
     * If the key is not found, an out_of_range exception is thrown.
     *
     * @param key_ The key to retrieve the associated value.
     * @return A reference to the value associated with the key.
     * @throws std::out_of_range if the key is not found.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::at(const KeyType &key_)
    {
        // Calculate the hash index
        auto hash_index = KeyHash()(key_) % m_size;

        // Iterate over the linked list at the hash position
        auto &hash_list = m_table[hash_index];

        // Check if the key already exists in the list
        auto it = std::find_if(hash_list.begin(), hash_list.end(),
                            [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });

        if (it != hash_list.end()) {
            return it->m_data;
        }

        throw std::out_of_range("Key not found");
    }

    /*!
     * @brief Access or insert the value associated with the specified key.
     *
     * This function accesses or inserts the value associated with the specified key.
     * If the key is found in the hash table, the corresponding value is returned.
     * If the key is not found, a new element with the specified key and a default-constructed
     * value is inserted into the hash table, and a reference to the newly inserted value is returned.
     *
     * @param key_ The key to access or insert.
     * @return A reference to the value associated with the key.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator[](const KeyType &key_)
    {
        // Calculate the hash index
        auto hash_index = KeyHash()(key_) % m_size;

        // Iterate over the linked list at the hash position
        auto &hash_list = m_table[hash_index];
        

        // Search for the element with the provided key in the list
        auto it = std::find_if(hash_list.begin(), hash_list.end(),
                            [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });

        // Check if the key already exists in the list
        if (it != hash_list.end()) {
            // If the key already exists, return a reference to the value associated with the key
            return it->m_data;
        } 
        else {
            // If the key does not exist, insert a new element into the table
            insert(key_, DataType{}); // Here you can use DataType{} or any desired default value
            // Search again for the newly inserted element
            hash_list.emplace_front(key_, DataType());
            return hash_list.front().m_data; // Return a reference to the newly inserted value
        }
    }
    /*!
     * @brief Set the maximum load factor for the hash table.
     *
     * This function sets the maximum load factor for the hash table.
     * The load factor is the ratio of the number of elements to the size of the table.
     * If the load factor exceeds the specified value during an insertion operation,
     * the table is rehashed to ensure proper performance.
     *
     * @param mlf The new maximum load factor.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::max_load_factor(float mlf)
    {
        m_factor_load = mlf;
    }

    /*!
     * @brief Get the current maximum load factor for the hash table.
     *
     * This function returns the current maximum load factor for the hash table.
     *
     * @return The current maximum load factor.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    float HashTbl<KeyType, DataType, KeyHash, KeyEqual>::max_load_factor() const
    {
        return m_factor_load;
    }



    /*=================== Métodos auxiliares ==============================*/
    /*!
     * @brief Initializes the hash table with the contents of another hash table.
     *
     * This function initializes the current hash table by copying the contents of another hash table.
     * It adjusts the size, count, and table structure to match the source hash table and inserts each
     * key-value pair from the source into the current hash table.
     *
     * @param source The source hash table to copy from.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::initialize_hash(const HashTbl& source) {

        
        // Copy the data from the source table
        m_size = source.m_size;
        m_count = source.m_count;

        // Allocate a new table with the adjusted size
        m_table = new std::forward_list<entry_type>[m_size];

        //m_table = source.m_table;

        //Iterate through the source table and insert each key-value pair into the current table
        for (size_t i{0}; i < source.m_size; ++i) {
            m_table[i] = source.m_table[i];
        }
    }


    /*!
     * @brief Initializes the hash table with the contents of an initializer list.
     *
     * This function initializes the current hash table by copying the contents of an initializer list.
     * It adjusts the size, count, and table structure to match the size of the initializer list and inserts
     * each key-value pair from the initializer list into the current hash table.
     *
     * @param ilist The initializer list containing key-value pairs.
     *
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::initialize_hash_ilist(
        const std::initializer_list<entry_type>& ilist) {
        
        m_size = m_count = ilist.size();
        
        // Set the size and count to match the initializer list size
        m_count = ilist.size();

        // Allocate a new table with the adjusted size
        m_table = new std::forward_list<entry_type>[m_size];

        // Iterate through the initializer list and insert each key-value pair into the current table
        for (const auto& entry : ilist) {
            insert(entry.m_key, entry.m_data);
        }
    }

    /*!
     * @brief Checks if a given number is a prime number.
     *
     * This function checks whether the provided number is a prime number.
     * It returns true if the number is prime and false otherwise.
     *
     * @param num The number to be checked for primality.
     * @return true if the number is prime, false otherwise.
     *
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::isPrime(size_type num) const {
        // Check if the number is less than or equal to 1
        if (num <= 1) {
            return false; // 1 is not a prime number, and negatives are not prime either
        }
        // Iterate from 2 to the square root of the number
        for (int i = 2; i * i <= num; ++i) {
            // If the number is divisible by any value in the range, it is not prime
            if (num % i == 0) {
                return false;
            }
        }
        // If no divisor is found, the number is prime
        return true;
    }

    /*!
     * @brief Finds the next prime number greater than the given number.
     *
     * This function finds the next prime number greater than the provided number.
     * It iteratively increments the given number until a prime number is found.
     *
     * @param num The number for which the next prime is to be found.
     * @return The next prime number greater than the provided number.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    int HashTbl<KeyType, DataType, KeyHash, KeyEqual>::nextPrime(size_type num) const {
        // Continue incrementing the number until a prime number is found
        while (true) {
            ++num;

            // If the current number is prime, return it
            if (isPrime(num)) {
                return num;
            }
        }
    }



} // Namespace ac.



