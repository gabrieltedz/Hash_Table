#include "hashtbl.h"


namespace ac {
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >

    /**
    * @brief Construtor que inicializa a tabela com um tamanho especificado.
    * 
    * @param sz Tamanho da tabela.
    */
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( size_type sz )
	{   
        m_size = sz;
        //Verifica se o tamanho escolido pelo usuário é primo, caso não seja busca o prómimo primo.
        if(!(isPrime(m_size))) m_size = nextPrime(sz);

        //Quantidades de dados é nulo.
        m_count = 0;

        //Inicializa o hash com o tamanho apropriado e primo.
        m_table = new std::forward_list<entry_type>[m_size];

	}

    /**
    * @brief Construtor que inicializa a tabela com base em outra tabela existente.
    * 
    * @param source A tabela da qual copiar os elementos.
    */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( const HashTbl& source )
	{
        // Função para inicializar o construtor, o mesmo usado com o operador =
        initialize_hash(source);

	}

    
    /**
     * @brief Construtor que inicializa a tabela com base em uma lista de inicialização.
     * 
     * @param ilist Lista de inicialização contendo pares chave-valor.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( const std::initializer_list<entry_type>& ilist )
    {
        // Função para inicializar o construtor, o mesmo usado com o operador =
        initialize_hash_ilist(ilist);
        
    }

    /**
     * @brief Operador de atribuição que substitui os elementos da tabela pelos de outra tabela.
     * 
     * @param clone A tabela a ser copiada.
     * @return Referência para a tabela após a cópia.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>&
    HashTbl<KeyType,DataType,KeyHash,KeyEqual>::operator=( const HashTbl& clone )
    {
        // Função para inicializar o hash com operador =, o mesmo usado com construtor 
        initialize_hash(clone);
        return *this;
    }

    /**
     * @brief Operador de atribuição que substitui os elementos da tabela pelos de uma lista de inicialização.
     * 
     * @param ilist Lista de inicialização contendo pares chave-valor.
     * @return Referência para a tabela após a cópia.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>&
    HashTbl<KeyType,DataType,KeyHash,KeyEqual>::operator=( const std::initializer_list< entry_type >& ilist )
    {
        // Função para inicializar o hash com operador =, o mesmo usado com construtor 
        initialize_hash_ilist(ilist);
        return *this;
    }

    /**
     * @brief Destrutor que libera a memória alocada pela tabela.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::~HashTbl( )
	{
        //deleta o hash.
        delete[] m_table;
	}

    /**
     * @brief Insere um novo par chave-valor na tabela.
     * 
     * @param key_ A chave a ser inserida.
     * @param new_data_ Os dados associados à chave.
     * @return true se a inserção foi bem-sucedida, false se a chave já existe.
     * 
     * Esta função insere um novo elemento na tabela hash. Calcula o índice hash para a chave
     * usando a função de hash e, em seguida, verifica se a chave já existe na lista encadeada
     * correspondente. Se a chave não existir, um novo par chave-valor é inserido no início da lista.
     * Se a carga na tabela exceder o fator de carga especificado, a função chama a rehash para
     * reorganizar a tabela e reduzir a carga.
     */
    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::insert(const KeyType &key_, const DataType &new_data_)
    {
        
        // Calcula o índice hash
        auto hash_index = KeyHash()(key_) % m_size;

        // Itera sobre a lista encadeada na posição hash_value
        auto & hash_list = m_table[hash_index];


        // Verifica se a chave já existe na lista
            auto it = std::find_if(hash_list.begin(), hash_list.end(),
                            [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });

        if (it == hash_list.end()) {
            // A chave não existe, adiciona um novo elemento na lista
            hash_list.emplace_front(key_, new_data_);
            m_count++;

            if(m_count / m_size > m_factor_load){
                rehash();
            }
            return true;  // Inserção bem-sucedida
        } 
        else {
            // A chave já existe, altera os dados

            it->m_data = new_data_;
            return false;  // Inserção mal-sucedida
        }



    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::clear()
    {
        // deleta os nós da listas encadeadas de cada índice
        for (size_t i{0}; i < m_size; ++i) {
            m_table[i].clear();
        }

        m_count = 0;
    }

    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::empty() const
    {
        return m_count == 0;
    }

    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::retrieve( const KeyType & key_, DataType & data_item_ ) const
    {
        // Itera sobre a lista encadeada na posição hash_value

        size_t hash_value = KeyHash()(key_) % m_size;

        
        auto &hash_list = m_table[hash_value];

        // Verifica se a chave já existe na lista
        auto it = std::find_if(hash_list.begin(), hash_list.end(),
                        [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });

        if (it != hash_list.end()) {
            // A chave já existe, adiciona um novo elemento na lista
            data_item_ = it->m_data;

            return true;  // Inserção bem-sucedida
        } 


        return false; 
    }

    /**
     * @brief Realiza o processo de rehashing, aumentando o tamanho da tabela hash e reorganizando os elementos.
     *
     * O rehashing é necessário quando o fator de carga da tabela hash atinge um limite, garantindo um desempenho adequado.
     * Neste processo, o tamanho da tabela hash é aumentado e todos os elementos são redistribuídos para as novas posições.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::rehash(void)
    {
        // Armazena o tamanho atual da tabela
        auto aux = m_size;

        // Verifica se o dobro do tamanho atual é um número primo
        if (!(isPrime(m_size * 2)))
            m_size = nextPrime(m_size * 2);

        // Cria uma nova tabela hash com o novo tamanho
        std::forward_list<entry_type> *m_table2 = new std::forward_list<entry_type>[m_size];

        // Muda os elementos da tabela antiga para a nova tabela
        for (size_t i = 0; i < aux; i++)
        {
            for (const auto &entry : m_table[i])
            {
                // Calcula o novo valor de hash para a chave
                size_t hash_value = KeyHash()(entry.m_key) % m_size;

                // Adiciona o elemento à nova posição na nova tabela
                m_table2[hash_value].emplace_front(entry.m_key, entry.m_data);
            }
        }

        // Limpa a tabela antiga
        delete [] m_table;

        // Atualiza a tabela para apontar para a nova tabela
        m_table = m_table2;
    }


    /**
     * @brief Remove um elemento com a chave fornecida da tabela hash.
     *
     * Esta função procura por um elemento na tabela hash com a chave especificada e o remove, se encontrado.
     *
     * @param key_ A chave do elemento a ser removido.
     * @return true se a remoção for bem-sucedida, false se a chave não for encontrada.
     */

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::erase(const KeyType &key_)
    {
        // Calcula o valor de hash para a chave
        size_t hash_value = KeyHash()(key_) % m_size;

        // Obtém a lista vinculada correspondente à posição de hash
        auto &hash_list = m_table[hash_value];

        // Procura o elemento com a chave fornecida na lista
        auto it = std::find_if(hash_list.begin(), hash_list.end(),
                                [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });

        // Verifica se algum elemento foi removido
        if (it != hash_list.end()) {
            // Remove os elementos removidos da lista
            hash_list.remove_if([&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });
            m_count--; // Atualiza o contador de elementos na tabela
            return true;
        }

        // A chave não foi encontrada na tabela hash
        return false;
    }

	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    typename HashTbl< KeyType, DataType, KeyHash, KeyEqual >::size_type
    HashTbl< KeyType, DataType, KeyHash, KeyEqual >::count( const KeyType & key_ ) const
    {
        return m_count;
    }

	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::at( const KeyType & key_ )
    {
        // Calcula o índice hash
        auto hash_index = KeyHash()(key_) % m_size;

        // Itera sobre a lista encadeada na posição hash_value
        auto & hash_list = m_table[hash_index];


        // Verifica se a chave já existe na lista
            auto it = std::find_if(hash_list.begin(), hash_list.end(),
                            [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });

        if (it != hash_list.end()) {
            return it->m_data;

        } 

        throw std::out_of_range("Key not found");
        

    }

    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator[](const KeyType &key_)
    {
        // Calcula o índice hash
        auto hash_index = KeyHash()(key_) % m_size;

        // Itera sobre a lista encadeada na posição hash_value
        auto &hash_list = m_table[hash_index];

        // Procura o elemento com a chave fornecida na lista
        auto it = std::find_if(hash_list.begin(), hash_list.end(),
                            [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });

        // Verifica se a chave já existe na lista
        if (it != hash_list.end()) {
            // Se a chave já existe, retorna uma referência para o valor associado à chave
            return it->m_data;
        } else {
            // Se a chave não existir, insere um novo elemento na tabela
            insert(key_, DataType{}); // Aqui você pode usar DataType{} ou qualquer valor padrão desejado
            // Procura novamente o elemento recém-inserido
            // it = std::find_if(hash_list.begin(), hash_list.end(),
            //                 [&](const entry_type &entry) { return KeyEqual()(entry.m_key, key_); });
            // // Retorna uma referência para o valor associado à chave
            // return it->m_data;
            hash_list.emplace_front(key_, DataType());
            return hash_list.front().m_data; // Retorna uma referência ao valor recém-inserido
        }
    }

    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::max_load_factor( float mlf )
    {
        m_factor_load = mlf;
    }

    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    float HashTbl<KeyType, DataType, KeyHash, KeyEqual>::max_load_factor() const
    {
        return m_factor_load;
    }




    /*=================== Métodos auxiliares ==============================*/
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::initialize_hash(const HashTbl& source  ){

        if(!isPrime(source.m_size)) m_size = nextPrime(source.m_size);
        m_count = source.m_count;
        m_table = new std::forward_list<entry_type>[m_size];

        for (size_t i{0}; i < source.m_size; ++i) {
            for (const auto& entry : source.m_table[i]) {
                insert(entry.m_key, entry.m_data);
            }
        }
    }


    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    void HashTbl<KeyType,DataType,KeyHash,KeyEqual>::initialize_hash_ilist( const std::initializer_list<entry_type>& ilist )
    {
        if(!isPrime(ilist.size())) m_size = m_count = nextPrime(ilist.size());
        m_size = m_count = ilist.size();
        m_table = new std::forward_list<entry_type>[m_size];

        for (const auto& entry : ilist) {
            insert(entry.m_key, entry.m_data);
        }
    }

    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType,DataType,KeyHash,KeyEqual>::isPrime(size_type num)const{
        if (num <= 1) {
            return false;
        }

        for (int i = 2; i * i <= num; ++i) {
            if (num % i == 0) {
                return false;
            }
        }

        return true;
    }

    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    int HashTbl<KeyType,DataType,KeyHash,KeyEqual>::nextPrime(size_type num)const{
        while (true) {
            ++num;
            if (isPrime(num)) {
                return num;
            }
        }
    }





} // Namespace ac.



