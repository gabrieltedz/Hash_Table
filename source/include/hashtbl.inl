#include "hashtbl.h"


namespace ac {
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( size_type sz )
	{
        m_size = sz;
        m_table = new std::forward_list<entry_type>[m_size];

	}

	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( const HashTbl& source )
	{
        for (size_t i{0}; i < ht_.m_size; ++i) {
            for (const auto& entry : ht_.m_table[i]) {
                insert(entry_type.m_key, entry_type.m_data);
            }
        }
        
	}

	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( const std::initializer_list<entry_type>& ilist )
    {
        auto run = ilist.begin();
        while (run != ilist.end()){
            //fazer

        }

        
    }

	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>&
    HashTbl<KeyType,DataType,KeyHash,KeyEqual>::operator=( const HashTbl& clone )
    {
        // TODO
        return *this;
    }

	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>&
    HashTbl<KeyType,DataType,KeyHash,KeyEqual>::operator=( const std::initializer_list< entry_type >& ilist )
    {
        // TODO
        return *this;
    }

	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::~HashTbl( )
	{
        delete[] m_table;
	}


    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::insert(const KeyType &key_, const DataType &new_data_)
    {
        // Calcular o índice hash
        auto hash_index = KeyHash()(key_) % m_size;

        // Iterar sobre a lista encadeada na posição hash_value
        auto & hash_list = m_table[hash_index];


        // Verificar se a chave já existe na lista
            auto it = std::find_if(hash_list.begin(), hash_list.end(),
                            [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });

        if (it == hash_list.end()) {
            // A chave não existe, adiciona um novo elemento na lista
            hash_list.emplace_front(key_, new_data_);
            m_count++;
            return true;  // Inserção bem-sucedida
        } 
        else {
            // A chave já existe, não faz nada (ou você pode decidir atualizar o valor)
            return false;  // Inserção mal-sucedida
        }


    }




    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::clear()
    {
        // TODO
    }

    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::empty() const
    {
        // TODO
        return false; // This is just a stub. Reaplace it accordinly.
    }

    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::retrieve( const KeyType & key_, DataType & data_item_ ) const
    {
        // TODO
        return false; // This is just a stub. Reaplace it accordinly.
    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::rehash( void )
    {
        // TODO
    }

	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl< KeyType, DataType, KeyHash, KeyEqual >::erase( const KeyType & key_ )
    {
        // TODO
        return false; // This is just a stub. Reaplace it accordinly.
    }


	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    std::size_t HashTbl<KeyType,DataType,KeyHash,KeyEqual>::find_next_prime( size_type  n_ )
    {
        // TODO
        return 0; // Stub
    }


	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    typename HashTbl< KeyType, DataType, KeyHash, KeyEqual >::size_type
    HashTbl< KeyType, DataType, KeyHash, KeyEqual >::count( const KeyType & key_ ) const
    {
        // TODO
        return 0; // Stub
    }

	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::at( const KeyType & key_ )
    {
        // TODO
        return m_table[0].begin()->m_data; // Stub
    }

	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator[]( const KeyType & key_ )
    {
        // TODO
        return m_table[0].begin()->m_data; // Stub
    }


} // Namespace ac.
