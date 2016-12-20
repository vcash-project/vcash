/*
 * Copyright (c) 2013-2016 John Connor (BM-NC49AxAjcqVcF5jNPu85Rb8MJ2d9JqZt)
 *
 * This file is part of vcash.
 *
 * vcash is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef COIN_BLOCK_HPP
#define COIN_BLOCK_HPP

#include <cstdint>
#include <vector>

#include <coin/data_buffer.hpp>
#include <coin/sha256.hpp>
#include <coin/transaction.hpp>

namespace coin {
    
    class block_index;
    class file;
    class key_store;
    class point_out;
    class tcp_connection;
    class tcp_connection_manager;
    class tx_db;
    
    /**
     * Implements a block.
     */
    class block : public data_buffer
    {
        public:

            /**
             * A block header.
             */
            typedef struct
            {
                std::uint32_t version;
                sha256 hash_previous_block;
                sha256 hash_merkle_root;
                std::uint32_t timestamp;
                std::uint32_t bits;
                std::uint32_t nonce;
            } header_t;
        
            /**
             * The header length.
             */
            enum { header_length = 80 };
    
            /**
             * The current version.
             */
            enum { current_version = 5 };

            /**
             * Constructor
             */
            block();
        
            /**
             * Encodes
             * @param block_header_only If true only the block header will be
             * encoded.
             */
            void encode(const bool & block_header_only = false);
        
            /**
             * Encodes
             * @param buffer The data_buffer.
             * @param block_header_only If true only the block header will be
             * encoded.
             */
            void encode(
                data_buffer & buffer, const bool & block_header_only = false
            );
        
            /**
             * Decodes
             * @param block_header_only If true only the block header will be
             * decoded.
             */
            bool decode(const bool & block_header_only = false);

            /**
             * Decodes
             * @param buffer The data_buffer.
             * @param block_header_only If true only the block header will be
             * decoded.
             */
            bool decode(
                data_buffer & buffer, const bool & block_header_only = false
            );
        
            /**
             * Sets null.
             */
            void set_null();

            /**
             * If true it is null.
             */
            bool is_null() const;
        
            /**
             * Gets the sha256 hash.
             */
            sha256 get_hash() const;
    
            /**
             * Get the sha256 (genesis) hash.
             */
            static sha256 get_hash_genesis();
        
            /**
             * Get the sha256 (genesis) hash for the test net.
             */
            static sha256 get_hash_genesis_test_net();

            /**
             * Returns the block size.
             */
            std::int64_t get_size();
        
            /**
             * The block header.
             */
            header_t & header();
        
            /**
             * The block header.
             */
            const header_t & header() const;
        
            /**
             * The transactions.
             * @note Used both in network and on disk.
             */
            std::vector<transaction> & transactions();
        
            /**
             * The signature.
             * @note Generated by the owner of the coin base txout[N].
             *
             */
            std::vector<std::uint8_t> & signature();
        
            /**
             * Updates the time.
             * @param previous The previous block index.
             */
            void update_time(block_index & previous);
        
            /**
             * Creates and returns the genesis block.
             */
            static block create_genesis();
        
            /**
             * Creates a block and fills it with transactions.
             * @param w The wallet.
             * @param proof_of_stake If true a Proof-of-Stake block will be
             * attempted to be created.
             */
            static std::shared_ptr<block> create_new(
                const std::shared_ptr<wallet> & w, const bool & proof_of_stake
            );
        
            /**
             * disconnect_block
             * @param tx_db The db_tx.
             * @param index The block_index.
             */
            bool disconnect_block(db_tx & tx_db, block_index * index);

            /**
             * connect_block
             * @param tx_db The db_tx.
             * @param check_only If true
             */
            bool connect_block(
                db_tx & tx_db, block_index * pindex,
                const bool  & check_only = false
            );
        
            /**
             * The entropy bit for stake modifier if chosen by modifier.
             * @param height The height.
             */
            std::uint32_t get_stake_entropy_bit(
                const std::uint32_t & height
            ) const;
    
            /**
             * If true it is proof of stake.
             */
            bool is_proof_of_stake() const;
        
            /**
             * If true it is proof of work.
             */
            bool is_proof_of_work() const;

            /**
             * Returns the proof of stake.
             */
            std::pair<point_out, std::uint32_t> get_proof_of_stake() const;

            /**
             * Returns the maximum transaction timestamp.
             */
            std::int64_t get_max_transaction_time() const;
        
            /**
             * Builds the merkle tree.
             */
            sha256 build_merkle_tree() const;
            
            /**
             * Checks the block.
             * @param connection The tcp_connection if any.
             * @param check_pow If true the proof-of-work will be checked.
             * @param check_merkle_root If true the merkle root will be checked.
             */
            bool check_block(
                const std::shared_ptr<tcp_connection> & connection = 0,
                const bool & check_pow = true,
                const bool & check_merkle_root = true
            );
        
            /**
             * Accepts a block into the main chain.
             * @param connection_manager The tcp_connection_manager used for
             * broadcasting a message to all ocnnected peers.
             */
            bool accept_block(
                const std::shared_ptr<tcp_connection_manager> &
                connection_manager
            );
        
            /**
             * Reads the block from disk.
             * @param index The block_index.
             * @param read_transactions If true the transctions will be read.
             */
            bool read_from_disk(
                const block_index * index,
                const bool & read_transactions = true
            );
    
            /**
             * Reads the block from disk.
             * @param file The file.
             * @param block_position The block position.
             * @param read_transactions If true the transctions will be read.
             */
            bool read_from_disk(
                const std::uint32_t & file_index,
                const std::uint32_t & block_position,
                const bool & read_transactions = true
            );

            /**
             * Writes to disk.
             * @param file_number The file number (on disk).
             * @param The block position.
             */
            bool write_to_disk(
                std::uint32_t & file_number, std::uint32_t & block_position
            );
        
            /**
             * Sets the best chain.
             * @param tx_db The db_tx.
             * @param index_new The block_index.
             */
            bool set_best_chain(db_tx & tx_db, block_index * index_new);
        
            /**
             * Adds the block to the block index.
             * @param file_index The file index.
             * @param block_position The block position.
             */
            bool add_to_block_index(
                const std::uint32_t & file_index,
                const std::uint32_t & block_position
            );
        
            /**
             * Attaches a block to the new best chain being built.
             * @param index_new The new block_index.
             */
            bool set_best_chain_inner(
                db_tx & tx_db, block_index * index_new
            );
        
            /**
             * Should be called when an invalid chain is found.
             * @param index_new The block_index.
             */
            static void invalid_chain_found(const block_index * index_new);
        
            /**
             * Signs (ppcoin).
             * @param store The key_store.
             */
            bool sign(const key_store & store);

            /**
             * Checks the signature.
             */
            bool check_signature() const;
    
            /**
             * Returns the maxium size of the next block based on 220 blocks
             * worth of size history (median).
             */
            static std::size_t get_maximum_size_median220();
            
            /**
             * Gets the block index file at the given index.
             * @param file_index The file index.
             */
            static std::string get_file_path(const std::uint32_t & file_index);

            /**
             * Opens a block file.
             * @param index The index.
             * @param position The position.
             * @param mode The mode.
             */
            static std::shared_ptr<file> file_open(
                const std::uint32_t & index, const std::uint32_t & position,
                const char * mode
            );
        
            /**
             * Opens a block file for appending.
             * @param index The index.
             */
            static std::shared_ptr<file> file_append(std::uint32_t & index);

            /**
             * Checks the proof-of-work.
             * @param hash The sha256.
             * @param bit The bits.
             */
            static bool check_proof_of_work(
                const sha256 & hash, const std::uint32_t & bits
            );
        
            /**
             * Gets a merkle branch.
             * @param index The index.
             */
            std::vector<sha256> get_merkle_branch(std::int32_t index) const;
        
            /**
             * Checks a merkle branch.
             * @param index The index.
             */
            static sha256 check_merkle_branch(
                sha256 h, const std::vector<sha256> & merkle_branch,
                std::int32_t index
            );
    
            /**
             * Prints the string representation of the block.
             */
            void print();
        
            /**
             * Runs the test case.
             */
            static int run_test();
        
        private:
        
            friend class block_index;
            friend class block_index_disk;

            /**
             * The block header.
             */
            header_t m_header;
        
            /**
             * The transactions.
             * @note Used both in network and on disk.
             */
            std::vector<transaction> m_transactions;
        
            /**
             * The signature.
             * @note Generated by the owner of the coin base txout[N].
             *
             */
            std::vector<std::uint8_t> m_signature;
        
            /**
             * THe merkle tree.
             */
            mutable std::vector<sha256> m_merkle_tree;
        
        protected:

            /**
             * Used to sort transactions by priority/fee when creating a new
             * block.
             */
            class transaction_fee_priority_compare
            {
                public:
                
                    /**
                     * Constructor
                     * @param sort_by_fee If true it will be sorted by fee.
                     */
                    transaction_fee_priority_compare(const bool & sort_by_fee)
                        : m_sort_by_fee(sort_by_fee)
                    {
                        // ...
                    }
                
                    /**
                     * operator ()
                     */
                    bool operator () (
                        const std::tuple<double, double, transaction *> & a,
                        const std::tuple<double, double, transaction *> & b
                        )
                    {
                        if (m_sort_by_fee)
                        {
                            if (std::get<1> (a) == std::get<1> (b))
                            {
                                return std::get<0> (a) < std::get<0> (b);
                            }
                            
                            return std::get<1> (a) < std::get<1> (b);
                        }
                        else
                        {
                            if (std::get<0> (a) == std::get<0> (b))
                            {
                                return std::get<1>(a) < std::get<1> (b);
                            }
                            
                            return std::get<0> (a) < std::get<0> (b);
                        }
                    }
                
                private:
                
                    /**
                     * If true it will sort by the fee.
                     */
                    bool m_sort_by_fee;

                protected:
                
                    // ...
            };
    };
    
} // namespace coin

#endif // COIN_BLOCK_HPP
