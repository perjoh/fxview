#pragma once
#include "../base/static_pow.hpp"
#include "shapes.hpp"
#include <array>
#include <vector>
#include <cassert>

namespace spatial {
namespace tree {

	// Highly specific container for storing items in segments/nodes.
	// Suitable for space partitioning data structures.
	// Number of segments can only increase, not decrease.
	template <typename Item, unsigned max_segments = 64, unsigned max_items = 256>
	class Linear_storage {
	public:
		size_t alloc_segment()
		{
			assert(num_segments < max_segments);

			segments_[num_segments] = Segment{ num_items, 0 };
			return num_segments++;
		}

	public:
		void insert(size_t segment, const Item& item)
		{
			expand_storage_by_one(segment);

			for (size_t i = segment + 1; i < num_segments; ++i)
				segments_[i].segment_start += 1;

			const size_t insert_at = segments_[segment].segment_start + segments_[segment].num_items;
			segments_[segment].num_items += 1;
			items_[insert_at] = item;

		}

		void remove(const Item& item)
		{
			const size_t segment_start = segments_[find_segment(item)].segment_start;
			const size_t item_index = &item - &items_[segment_start];
			remove(find_segment(item), item_index);
		}

		//void move(const Item& item, size_t to_segment);

		//void remove(size_t segment, size_t item_index);
		//void move(size_t from_segment, size_t item_index, size_t to_segment);

		//Item& get_item(size_t segment, size_t item_index);
		//const Item& get_item(size_t segment, size_t item_index) const;

		//size_t num_items() const;
		//size_t num_items(size_t segment) const;

	public:
		// Call f for each item in segment
		//template <typename Fun>
		//void for_each(size_t segment, const Fun& f);

		// Call f for each item in whole container
		//template <typename Fun>
		//void for_each(const Fun& fun);

	private:
		//
		//
		//
		size_t find_segment(const Item& item) const
		{
			// Binary search for segment

			const size_t item_offset = &item - &items_[0];
			size_t segment = max_items / 2;
			size_t step = segment;
			while (segments_[segment].is_offset_inside(item_offset))
			{
				assert(step != 0);

				step /= 2;
				if (item_offset < segments_[segment].segment_start)
					segment -= step;
				else
					segment += step;
			}

			return segment;
		}

		//
		//
		//
		void expand_storage_by_one(size_t segment)
		{
			/*std::copy_backward(	std::begin(&items_[segments_[segment].segment_begin]),
								std::end(&items_[num_items]),
								std::end(&items_[num_items + 1]));*/

			copy_backwards(segment + 1);
		}

		//
		//
		//
		void copy_backwards(size_t segment)
		{
			if (segment < num_segments)
			{
				copy_backwards(segment + 1);
				const size_t from = segments_[segment].segment_begin;
				const size_t to = segments_[segment + 1].segment_begin;
				items_[to] = items_[from];
			}
		}

	private:
		size_t num_segments_ = 0;

		struct Segment {
			short segment_start = 0;
			short num_items = 0;

			inline const bool is_offset_inside(size_t offset) const
			{
				return !(offset < segment_start) && offset < (segment_start + num_items);
			}
		};
		Segment segments_[max_items];

		size_t num_items = 0;
		Item items_[max_items];
	};

	//
	// Fixed node size. Dynamic item capacity(up to 65535).
	//
	template <typename Item, unsigned max_depth = 3>
	class Quad_tree
	{
		// NOTES:
		//

		using Rectangle = spatial::shapes::Rectangle<decltype(Item.x)>;

	public:
		//static const unsigned num_preallocated_items = 256;
		static const unsigned max_items_per_node = 32;

		Quad_tree(const Rectangle& root_rect)
			: items_(num_preallocated_items)
		{
			initialize(0, root_rect);
		}

	public:
		//
		//
		//
		void insert(const Item& item)
		{
			add_item(item, find_best_fit(item, 0, 0));
		}

		//
		//
		//
		void remove(const Item& item)
		{
			const composite_index_type index = find_best_fit(item, 0, 0);
			if (index != invalid_index)
			{
				const unsigned node_index = get_node_index(index);
				node_type& node = nodes_[node_index];
				const unsigned item_index = get_item_index(index);
				for (unsigned i = item_index;
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

	public:
		template <typename Fun>
		void integrate_items(Fun& fun)
		{
			for (auto item : items_)
			{
				fun(item);
			}

			refresh_recursive(0, 0);
		}

	public:
		template <	typename Shape,
			typename Fun>
			void for_each_intersecting(const Shape& shape,
									   Fun& fun) const
		{
			// TODO
		}

	private:
		//
		composite_index_type find_best_fit(const Item& item,
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
						const unsigned composite_index = find_best_fit(item,
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
		void refresh_recursive(unsigned node_index,
							   unsigned item_offset)
		{
			node_type& node = nodes_[node_index];
			const bool is_leaf = is_leaf(node_index);

			// Iterate all items in current node.
			const unsigned max_item_offset = item_offset + node.item_count.second();
			for (unsigned item_index = item_offset;
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
							const composite_index_type index = find_best_fit(item,
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

	private:
		static const unsigned invalid_index = ~(0ul);

		struct Node
		{
			Rectangle rect;
			short item_count{ 0 };
			short storage_segment{ 0 };
		};

		//
		std::array<node, base::static_pow<4, max_depth>> nodes_;

		Linear_storage<Item> items_;

		//
		void initialize(unsigned node_index,
						const Rectangle& rect)
		{
			nodes_[node_index].rect = rect;

			if (!is_leaf(node_index))
			{
				const std::array<Rectangle, 4> sub_rects{ parent_rect.split() };
				initialize(get_child_index(node_index) + 0, sub_rects[0]);
				initialize(get_child_index(node_index) + 1, sub_rects[1]);
				initialize(get_child_index(node_index) + 2, sub_rects[2]);
				initialize(get_child_index(node_index) + 3, sub_rects[3]);
			}
		}

		//
		bool insert_internal(const Item& item,
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

	private:
		void add_item(node_type& node,
					  const Item& item)
		{
			if (invalid_index == node.storage_segment)
			{
				node.storage_segment = items_.alloc_segment();
			}

			items_.add_item(node.storage_segment, item);
		}

		void remove_item(node_type& node,
						 unsigned item_index)
		{
			items_.remove_item(node.storage_segment, item_index);
		}

	public:
		// If nodes are placed in a flat array, child nodes is calculated as: i*4 + 4 if i is the current node index.

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
			return !(child_index(node_index) < nodes_.size());
		}
	};

} }
