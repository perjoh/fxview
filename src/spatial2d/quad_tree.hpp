#pragma once
#include "../base/static_pow.hpp"
#include "primitives_2d.hpp"
#include <array>
#include <vector>
#include <cassert>

namespace spatial2d
{

	// Fixed node size. Dynamic item capacity(up to 65535).
	template <typename item_type, unsigned max_depth>
	class quad_tree
	{
	public :
		static const unsigned num_preallocated_items = 256;

		quad_tree(const rectangle& root_rect)
			: items_(num_preallocated_items)
		{
			initialize(0, root_rect);
		}

	public :
		//
		void insert(const item_type& item)
		{
			add_item(item, find_best_fit(item, 0, 0));
		}

		//
		void remove(const item_type& item)
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

		template <typename fun_type>
		void integrate_items(fun_type& fun)
		{
			for (auto item : items_)
			{
				fun(item);
			}

			refresh_recursive(0, 0);
		}

	private :
		static inline unsigned get_node_index(unsigned composite_index)
		{
			return composite_index&0xffff;
		}

		static inline unsigned get_item_index(unsigned composite_index)
		{
			return (composite_index >> 16)&0xffff;
		}

		typedef unsigned composite_index_type;

		static inline composite_index_type get_composite_index(	unsigned node_index, 
																unsigned item_index)
		{
			return (node_index&0xffff) | ((item_index&0ffff) << 16);
		}

		//
		composite_index_type find_best_fit(	const item_type& item,
											unsigned node_index,
											unsigned item_offset) const
		{
			const node_type& node = nodes_[node_index];

			if (node.rect.contains(item.bounding_shape()))
			{
				if (!is_leaf(node_index))
				{
					const unsigned child_node_index = get_child_index(node_index);
					const unsigned child_item_offset = item_offset + node.item_count;

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

			for (	unsigned item_index = item_offset;
					item_index < item_offset + node.item_count;
					++item_index)
			{
				const item_type item = items_[item_index];
				if (node.rect.contains(item.bounding_shape()))
				{
					if (!is_leaf)
					{
						// Item still fits in current node. Check if it fits better in any child node.
						const unsigned child_index = get_child_index(node_index);
						const unsigned child_item_offset = item_offset + node.item_count;

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

		// Store items in a linear array. Locations are dictated by the parent nodes recursive sum of item_count.
		typedef std::vector<item_type> item_array;
		item_array items_;

		struct node_type
		{
			rectangle rect;
			unsigned item_count{0};
		};

		//
		std::array<node, static_pow<4, max_depth>::result> nodes_;

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
		bool insert_internal(	const item_type& item,
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
		void add_item(const item_type& item, composite_index_type index)
		{
			add_item(	item, 
						get_node_index(index), 
						get_item_index(index));
		}

		void add_item(	const item_type& item, 
						unsigned node_index, 
						unsigned item_index)
		{
			nodes_[node_index].item_count += 1;
			items_.insert(items_.begin() + item_index, item);
		}

		void remove_item(	unsigned node_index, 
							unsigned item_index)
		{
			nodes_[node_index].item_count -= 1;
			items_.erase(items_.begin() + item_index);
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
		quad_tree<rectangle, 4> tree;
	}

}
