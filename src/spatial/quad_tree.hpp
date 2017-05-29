#pragma once
#include "../base/static_pow.hpp"
#include "shapes.hpp"
#include <array>
#include <vector>
#include <cassert>

namespace kvant {
namespace spatial {

    template <typename Item>
    class Block_storage {
    public :
        Block_storage()
        {
            assert(blocks_[0].capacity() == 0);
        }

    public :
        unsigned alloc_block()
        {
            for (unsigned i = 0; i < num_blocks; ++i)
            {
                if (blocks_[i].capacity() == 0)
                {
                    blocks_[i].reserve(default_block_size); 
                    return i;
                }
            }
        }

        void add(unsigned block_index, const Item& item)
        {
            blocks_[block_index].push_back(item);
        }

        void remove(unsigned block_index, const Item& item) 
        {
            Block& block = blocks_[block_index];
            for (size_t i = 0; i < block.size(); ++i)
            {
                if (&block[i] == &item)
                {
                    block.erase(block.begin() + i);
                    return;
                }
            }
        }

        template <typename Fun>
        void remove_if_not(Fun&& fun)
        {
            for (size_t i = 0; i < num_blocks; ++i)
            {
                Block& block = blocks_[i];
                if (block.capacity() == 0)
                {
                    return;
                }

                for (auto it = block.begin(); it != block.end();)
                {
                    if (!fun(*it))
                    {
                        it = block.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                } 
            }
        }

        template <typename Fun>
        void for_each_item_in_block(Fun&& fun, unsigned block_index)
        {
            for (auto& item : blocks_[block_index])
            {
                fun(item);
            }
        }

    private :
        using Block = std::vector<Item>;
        const unsigned num_blocks = 64;
        const unsigned default_block_size = 8;
        Block blocks_[num_blocks];
    }; 

	//
	template <typename Item, typename Storage = Block_storage>
	class Quad_tree
	{
		// NOTES:
		//

		using Rectangle = spatial::shapes::Rectangle<float>;
        const unsigned max_depth{3};

	public: 
		Quad_tree(const Rectangle& root_rect)
		{
			initialize(0, root_rect);
		}

	public:
		//
		//
		//
		void insert(const Item& item)
		{
			add_item(find_best_fit(item, 0, 0), item);
		} 

	public:
		template <typename Fun>
		void integrate_items(Fun& fun)
		{
            items_.remove_if_not(fun); 
		}

	public:
		template <typename Shape, typename Fun>
        void for_each_intersecting(const Shape& shape, Fun& fun) const
		{
			// TODO
		}

	private:
		//
		unsigned find_best_fit(const Item& item, unsigned node_index) const
		{
			const Node& node = nodes_[node_index];

			if (node.contains(item.bounding_shape()))
			{
				if (!is_leaf(node_index))
				{
					const unsigned child_node_index = get_child_index(node_index);

                    // Check if there's a better fit among the child nodes.
					for (unsigned i = 0; i < 4; ++i)
					{
						const unsigned tmp = find_best_fit(item, child_node_index + i); 
						if (tmp != invalid_index)
						{
							return tmp;
						}
					}
				}

                return node_index;
			}

			return invalid_index;
		} 

	private:
		static const unsigned invalid_index = ~(0ul);

		struct Node
		{
			Rectangle rect;
			short item_count{ 0 };
			short storage_id{ 0 };

            template <typename T>
            bool contains(const T& shape)
            {
                return rect.contains(shape); 
            }
		};

		//
		std::array<node, base::static_pow<4, max_depth>> nodes_;

		Storage<Item> items_;

		//
		void initialize(unsigned node_index,
						const Rectangle& rect)
		{
			nodes_[node_index].rect = rect;

			if (!is_leaf(node_index))
			{
				const std::array<Rectangle, 4> sub_rects{ rect.split() };
				initialize(get_child_index(node_index) + 0, sub_rects[0]);
				initialize(get_child_index(node_index) + 1, sub_rects[1]);
				initialize(get_child_index(node_index) + 2, sub_rects[2]);
				initialize(get_child_index(node_index) + 3, sub_rects[3]);
			}
		}

	private:
		void add_item(unsigned node_index, const Item& item)
		{
            Node& node = nodes_[node_index];

            if (node.storage_id == invalid_index)
            {
                node.storage_id = items_.alloc_block();
            }

            items_.add(node.storage_id, item);
            node.item_count += 1;
		}

		void remove_item(unsigned node_index, const Item& item)
		{
            Node& node = nodes_[node_index];
			items_.remove(node.storage_segment, item);
            node.item_count -= 1;
		}

	private:
		// If nodes are placed in a flat array, child nodes is calculated as: i*4 + 1 if i is the current node index.

		static unsigned get_child_index(unsigned parent_index)
		{
			assert(parent_index != invalid_index);
			return parent_index * 4 + 1;
		}

		static unsigned get_parent_index(unsigned child_index)
		{
			assert(child_index != invalid_index);
			return (child_index - 1) / 4;
		}

		bool is_leaf(unsigned node_index) const
		{
			return !(get_child_index(node_index) < nodes_.size());
		}
	};

} }
