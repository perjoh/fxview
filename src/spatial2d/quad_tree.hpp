#pragma once
#include "static_pow.hpp"
#include "primitives_2d.hpp"
#include <array>
#include <vector>
#include <cassert>

namespace tree {

	template <typename Item, unsigned block_size = 8>
	class Block_storage {
	public :
		typedef unsigned Index;

		Index alloc_block()
		{
			blocks_.push_back(Block()); 
			alloc_entries_.push_back(Alloc_entry(blocks_.size() - 1)); 
			return alloc_entries_.size() - 1;
		}

		void add_item(Index block_index, const Item& item)
		{
			Alloc_entry& entry = alloc_entries_[block_index];

			if (entry.count == entry.capacity)
			{ 
				blocks_.insert(blocks_.begin() + entry.index, Block());
				entry.capacity += block_size;

				for (auto& e : alloc_entries_)
				{
					if (block_index < e.index)
						e.index += 1;
				}
			} 

			blocks_[block_index][entry.count] = item;
			entry.count += 1;
		}

		std::pair<Item*, Item*> get_items(Index block_index)
		{
			const Alloc_entry& entry = alloc_entries_[block_index];
			Item* item = blocks_[entry.index];
			return std::make_pair(item, item + entry.count);
		}

	public :
		template <typename Fun>
		void for_each(Fun& fun)
		{
			for (const auto& entry : alloc_entries_)
			{
				if (entry.count != 0)
				{
					auto range = get_items(entry.index);
					for (Item* item = range.first; item != range.second; ++item)
					{
						fun(*item);
					}
				}
			}
		}

	private :
		void remove_item(Index block_index, unsigned item_index)
		{
			const unsigned new_item_count = alloc_entries_[block_index].item_count - 1;
			alloc_entries_[block_index].item_count = new_item_count;
			if (item_index != new_item_count && new_item_count != 0)
				blocks_[block_index][item_index] = blocks_[block_index][new_item_count]; 
		}

	private : 
		struct Alloc_entry {
			Alloc_entry(unsigned i)
				: index(i)
			{ }

			unsigned index{0};
			unsigned count{0};
			unsigned capacity{block_size};
		};

		std::vector<Alloc_entry> alloc_entries_; 

		typedef std::array<Item, block_size> Block;
		std::vector<Block> blocks_;
	};

	// Fixed node size. Dynamic item capacity(up to 65535).
	template <typename Item, unsigned max_depth>
	class Quad_tree
	{
		// NOTES:
		//

	public :
		//static const unsigned num_preallocated_items = 256;
		static const unsigned max_items_per_node = 32;

		Quad_tree(const rectangle& root_rect)
			: items_(num_preallocated_items)
		{
			initialize(0, root_rect);
		}

	public :
		//
		void insert(const Item& item)
		{
			add_item(item, find_best_fit(item, 0, 0));
		}

		//
		void remove(const Item& item)
		{
			const composite_index_type index = find_best_fit(item, 0, 0);
			if (index != invalid_index)
			{
				const unsigned node_index = get_node_index(index);
				node_type& node = nodes_[node_index];
				const unsigned item_index = get_item_index(index);
				for (	unsigned i = item_index;
						i != item_index + node.item_count;
						++i)
				{
					if (items_[i] == item)
					{
						remove_item(node_index, i);
						return;
					}
				}
			}
		}

	public :
		template <typename Fun>
		void integrate_items(Fun& fun)
		{
			for (auto item : items_)
			{
				fun(item);
			}

			refresh_recursive(0, 0);
		}

	public :
		template <	typename Shape, 
					typename Fun>
		void for_each_intersecting(	const Shape& shape, 
									Fun& fun) const
		{
			// TODO
		}

	private :
		//
		composite_index_type find_best_fit(	const Item& item,
											unsigned node_index,
											unsigned item_offset) const
		{
			const node_type& node = nodes_[node_index];

			if (node.rect.contains(item.bounding_shape()))
			{
				if (!is_leaf(node_index))
				{
					const unsigned child_node_index = get_child_index(node_index);
					const unsigned child_item_offset = item_offset + node.item_count.second();

					for (unsigned i = 0; i < 4; ++i)
					{
						const unsigned composite_index = find_best_fit(	item, 
																		child_node_index + i, 
																		child_item_offset);

						if (composite_index != invalid_index)
						{
							return composite_index;
						}
					}
				}

				return get_composite_index(node_index, item_offset);
			}

			return invalid_index;
		}

		// Makes sure items belongs to the correct node.
		void refresh_recursive(	unsigned node_index,
								unsigned item_offset)
		{
			node_type& node = nodes_[node_index];
			const bool is_leaf = is_leaf(node_index);

			// Iterate all items in current node.
			const unsigned max_item_offset = item_offset + node.item_count.second();
			for (	unsigned item_index = item_offset;
					item_index < max_item_offset;
					++item_index)
			{
				const Item item = items_[item_index];
				if (node.rect.contains(item.bounding_shape()))
				{
					if (!is_leaf)
					{
						// Item still fits in current node. Check if it fits better in any child node.
						const unsigned child_index = get_child_index(node_index);
						const unsigned child_item_offset = max_item_offset; // item_offset + node.item_count;

						for (unsigned i = 0; i < 4; ++i)
						{
							const composite_index_type index = find_best_fit(	item, 
																				child_index + i, 
																				child_item_offset);

							if (index != invalid_index)
							{
								// NOTE Can be optimized better. A move would be one buffer copy instead of two.
								// NOTE Items moved down the tree will be checked again. (Redundancy)
								// Order important.
								add_item(item, index);
								remove_item(node_index, item_index);
								break;
							}
						}
					}
				}
				else
				{
					// Item does not fit in node, re-add it from top.
					add_item(item, find_best_fit(item, 0, 0));
					remove_item(node_index, item_index);

					item_offset += 1;
				}
			}

			if (!is_leaf)
			{
				const unsigned child_index = get_child_index(node_index);
				const unsigned child_item_offset = item_offset + node.item_count;

				refresh_recursive(child_index + 0, child_item_offset);
				refresh_recursive(child_index + 1, child_item_offset);
				refresh_recursive(child_index + 2, child_item_offset);
				refresh_recursive(child_index + 3, child_item_offset);
			}
		}

	private :
		static const unsigned invalid_index = ~(0ul); 

		struct node_type
		{
			rectangle rect;
			unsigned item_count{0}; 
			unsigned storage_segment{invalid_index};
		};

		//
		std::array<node, static_pow<4, max_depth>::result> nodes_;

		Block_storage items_;

		//
		void initialize(unsigned node_index,
						const rectangle& rect)
		{
			nodes_[node_index].rect = rect;

			if (!is_leaf(node_index))
			{
				const std::array<rectangle, 4> sub_rects{parent_rect.split()};
				initialize(get_child_index(node_index) + 0, sub_rects[0]);
				initialize(get_child_index(node_index) + 1, sub_rects[1]);
				initialize(get_child_index(node_index) + 2, sub_rects[2]);
				initialize(get_child_index(node_index) + 3, sub_rects[3]);
			}
		}

		//
		bool insert_internal(	const Item& item,
								unsigned node_index)
		{
			node_type& node = nodes_[node_index];
			if (node.rect.contains(item.bounding_shape()))
			{
				if (is_leaf(node_index) ||
						(!insert_internal(item, get_child_index(node_index + 0)) &&
						 !insert_internal(item, get_child_index(node_index + 1)) &&
						 !insert_internal(item, get_child_index(node_index + 2)) &&
						 !insert_internal(item, get_child_index(node_index + 3))))
				{
					node.items.push_back(item); // FIXME push_back or find empty spot?
					return true;
				}
			}

			return false;
		}

	private :
		void add_item( 	node_type& node,
						const Item& item)
		{
			if (invalid_index == node.storage_segment)
			{
				node.storage_segment = items_.alloc_segment();
			}

			items_.add_item(node.storage_segment, item);
		}

		void remove_item(	node_type& node,
							unsigned item_index)
		{
			items_.remove_item(node.storage_segment, item_index);
		}

	public :
		// If nodes are placed in a flat array, child nodes is calculated as: i*4 + 4 if i is the current node index.

		static unsigned get_child_index(unsigned parent_index)
		{
			assert(parent_index != invalid_index);
			return parent_index*4 + 1;
		}

		static unsigned get_parent_index(unsigned child_index)
		{
			assert(child_index != invalid_index);
			return (child_index - 1)/4;
		}

		bool is_leaf(unsigned node_index) const
		{
			return !(child_index(node_index) < nodes_.size());
		} 
	};

	void test_insert()
	{
		Quad_tree<rectangle, 4> tree;
	}

}
