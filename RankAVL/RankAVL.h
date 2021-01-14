#ifndef _RANK_AVL_T
#define _RANK_AVL_T
#include <memory>
#include <assert.h>
#include "AVL.h"
#include "../Exceptions/Exceptions.h"

namespace DS
{
    enum class SearchPath
    {
        start, left, right, parent, end
    };

    template<typename RANK, typename KEY_TYPE=int, typename VAL_TYPE=int, class NODE=graph_node<KEY_TYPE, VAL_TYPE>> 
    class RankAVL : public AVL<KEY_TYPE, VAL_TYPE, NODE>
    {
        typedef AVL<KEY_TYPE, VAL_TYPE, NODE> Avl;
    private:
        /*********************************/
        /*        Private Section        */
        /*********************************/
        RANK rankUpdate;

        /*   Class Private Methods   */
        // General right and left rotations for balanced trees, return the new root of the tree.
        std::shared_ptr<NODE> rotateRight(std::shared_ptr<NODE>& sub_root) override
        {
            std::shared_ptr<NODE> father = sub_root->father;
            std::shared_ptr<NODE> L_sub = sub_root->left;
            std::shared_ptr<NODE> LR_sub = L_sub->right;

            // Do the rotation:
            L_sub->right = sub_root;
            sub_root->left = LR_sub;
            L_sub->father = father; // Update the fathers
            sub_root->father = L_sub;
            if(LR_sub)
            {
                LR_sub->father = sub_root;
            }

            // Update the new heights:
            sub_root->height = Avl::max(Avl::height(sub_root->left), Avl::height(sub_root->right)) + 1;
            L_sub->height = Avl::max(Avl::height(L_sub->left), Avl::height(L_sub->right)) + 1;

            // Update ranks:
            rankUpdate(sub_root);
            rankUpdate(L_sub);

            // Return the new sub root
            return L_sub;
        }


        std::shared_ptr<NODE> rotateLeft(std::shared_ptr<NODE>& sub_root) override
        {
            std::shared_ptr<NODE> father = sub_root->father;
            std::shared_ptr<NODE> R_sub = sub_root->right;
            std::shared_ptr<NODE> RL_sub = R_sub->left;

            // Do the rotation:
            R_sub->left = sub_root;
            sub_root->right = RL_sub;
            R_sub->father = father; // Update the fathers
            sub_root->father = R_sub;
            if(RL_sub)
            {
                RL_sub->father = sub_root;
            }

            // Update the new heights:
            sub_root->height = Avl::max(Avl::height(sub_root->left), Avl::height(sub_root->right)) + 1;
            R_sub->height = Avl::max(Avl::height(R_sub->left), Avl::height(R_sub->right)) + 1;

            // Update ranks:
            rankUpdate(sub_root);
            rankUpdate(R_sub);

            // Return the new sub root
            return R_sub;
        }

        std::shared_ptr<NODE> insertAux(const KEY_TYPE& key, const VAL_TYPE& val, std::shared_ptr<NODE>& root) override
        {
            assert(root != nullptr);
            // 1. Do a normal BST rotation:
            if(root->key > key)
            {
                if(root->left == nullptr)
                {
                    root->left = Avl::newNode(key, val, root);
                    root->height = Avl::max(Avl::height(root->left), Avl::height(root->right)) + 1;
                    rankUpdate(root->left); // Update ranks
                    rankUpdate(root);
                    Avl::node_count++;
                    return root;
                }
                root->left = insertAux(key, val, root->left);
            }
            else if(root->key < key)
            {
                if(root->right == nullptr)
                {
                    root->right = Avl::newNode(key, val, root);
                    root->height = Avl::max(Avl::height(root->left), Avl::height(root->right)) + 1;
                    rankUpdate(root->right); // Update ranks
                    rankUpdate(root);
                    Avl::node_count++;
                    return root;
                }
                root->right = insertAux(key, val, root->right);
            }
            else //There already exists a node with the same key, so overwrite it's contents.
            {
                root->val = val;
                return root;
            }

            // 2. Update the height and ranks of all the nodes in the BST search.
            root->height = Avl::max(Avl::height(root->left), Avl::height(root->right)) + 1;
            rankUpdate(root);

            // 3. Verify balance factors, and perform rotations if needed.
            int balance_fact = Avl::getBalance(root);
            // RR Rotation:
            if((balance_fact < -1) && (root->right->key < key))
            {
                return rotateLeft(root);
            }
            // LL Rotation:
            if((balance_fact > 1) && (root->left->key > key))
            {
                return rotateRight(root);
            }
            // RL Rotation:
            if((balance_fact < -1) && (root->right->key > key))
            {
                root->right = rotateRight(root->right);
                return rotateLeft(root);
            }
            // LR Rotation:
            if((balance_fact > 1) && (root->left->key < key))
            {
                root->left = rotateLeft(root->left);
                return rotateRight(root);
            }
            return root;
        }

        // An auxiliary eraese method for the class' use.
        std::shared_ptr<NODE> eraseAux(std::shared_ptr<NODE>& root, const KEY_TYPE& key) override
        {
            // Search the node in BST
            if (root == nullptr)
            {
                return root;
            }
            if (key < root->key)
            {
                root->left = eraseAux(root->left, key);
            }
            else if (key > root->key)
            {
                root->right = eraseAux(root->right, key);
            }
            // If root->key==key then that's the node we want to delete
            else
            {
                // If the root has up to 1 child:
                if(root->left == nullptr || root->right == nullptr)
                {
                    std::shared_ptr<NODE> child = root->left? root->left : root->right;

                    // If there are no children at all
                    if (child==nullptr)
                    {
                        root = nullptr;
                        Avl::node_count--; // Decrement the count of nodes in the tree
                    }
                    // Only one child
                    else
                    {
                        child->father = root->father;
                        *(root) = *(child);
                        // Update the children's father to his new address:
                        if(child->right)
                        {
                            child->right->father = root;
                        }
                        if(child->left)
                        {
                            child->left->father = root;
                        }
                        AVL<KEY_TYPE, VAL_TYPE, NODE>::node_count--; // Decrement the count of nodes in the tree
                    }
                }
                // Two children
                else
                {
                    // Find the root's replacement node:
                    std::shared_ptr<NODE> nextMin = Avl::findLowestNode(root->right);
                    // Backup the pointers of root before we override them
                    std::shared_ptr<NODE> rootOldLeftChild = root->left;
                    std::shared_ptr<NODE> rootOldRightChild = root->right;
                    std::shared_ptr<NODE> rootOldFather = root->father;
                    *(root) = *(nextMin); // Overwrite root with his replacement
                    root->left = rootOldLeftChild; // Restore the original pointers
                    root->father = rootOldFather;
                    root->right = rootOldRightChild;
                    root->right = eraseAux(root->right, nextMin->key);
                }
            }

            // If a leaf was deleted:
            if (root == nullptr)
            {
                return root;
            }
            // Update heights and ranks:
            root->height = Avl::max(Avl::height(root->right), Avl::height(root->left)) + 1;
            rankUpdate(root);
            // Confirm balance factors:
            int balance_fact = Avl::getBalance(root);
            // If the node is unbalanced then we need two rotations according to the 4 cases:
            if (balance_fact > 1)
            {
                if (Avl::getBalance(root->left) >= 0) // LL rotation
                {
                    return rotateRight(root);
                }
                else // LR rotation
                {
                    root->left = rotateLeft(root->left);
                    return rotateRight(root);
                }
            }
            if (balance_fact < -1)
            {
                if (Avl::getBalance(root->right) <= 0)  //RR rotation
                {
                    return rotateLeft(root);
                }
                else //RL rotation
                {
                    root->right = rotateRight(root->right);
                    return rotateLeft(root);
                }
            }
            return root;
        }

    public:
        /**********************************/
        /*         Public Section         */
        /**********************************/
        /*
         * Constructor: RankAVL
         * Usage: RankAVL<RANK, KEY_TYPE, VAL_TYPE, NODE> tree(root, rankUpdate);
         *        RankAVL<RANK, KEY_TYPE, VAL_TYPE, NODE> tree(rankUpdate);
         * ---------------------------------------
         * Create an empty Rank AVL tree, or intialize it with a root using the first syntax.
         * Worst time complexity: O(1)
         * 
         * Possible Exceptions:
         * std::bad_alloc
         */
        explicit RankAVL(const NODE& root, RANK func) : Avl::AVL(root), Avl::rankUpdate(func) { }
        explicit RankAVL(RANK func) : Avl::AVL(), rankUpdate(func) { }

        RankAVL(const AVL<KEY_TYPE, VAL_TYPE,NODE>& other) = delete;
        RankAVL& operator=(const RankAVL& other) = delete;
        virtual ~RankAVL() = default;

        /*
         * Method: insert
         * Usage: tree.insert(key, val);
         * -----------------------------------
         * Inserts the pair (key, val) to the AVL Ranks tree.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(log n).
         * 
         * Possible Exceptions:
         * std::bad_alloc
         */
        void insert(const KEY_TYPE& key, const VAL_TYPE& val) override
        {
            if(Avl::tree_root == nullptr)
            {
                Avl::tree_root = Avl::newNode(key, val);
                rankUpdate(Avl::tree_root); // Update rank
                Avl::leftmost_node = Avl::rightmost_node = Avl::tree_root;
                Avl::node_count++;
                return;
            }
            Avl::tree_root = insertAux(key, val, Avl::tree_root);
            
            assert(Avl::leftmost_node);
            assert(Avl::rightmost_node);
            if(key < Avl::leftmost_node->key)
            {
                Avl::leftmost_node = Avl::findLowestNode(Avl::tree_root);
            }
            if(key > Avl::rightmost_node->key)
            {
                Avl::rightmost_node = Avl::findHighestNode(Avl::tree_root);
            }
        }

        /*
         * Method: erase
         * Usage: tree.erase(key);
         * -----------------------------------
         * Erases the pair corresponding to 'key' from the AVL Rank tree.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(log n).
         */
        void erase(const KEY_TYPE& key) override
        {
            Avl::tree_root = eraseAux(Avl::tree_root, key);
            if(Avl::tree_root)
            {
                assert(Avl::leftmost_node);
                assert(Avl::rightmost_node);
                Avl::leftmost_node = Avl::findLowestNode(Avl::tree_root);
                Avl::rightmost_node = Avl::findHighestNode(Avl::tree_root);
            }
            else
            {
                Avl::leftmost_node = nullptr;
                Avl::rightmost_node = nullptr;
            }
        }

        /*
         * Method: rank
         * Usage: tree.rank(key, calc_functor);
         * -----------------------------------
         * Call the calc_functor for each step in the search path to allow
         * the user to calculate the rank of the corresponding key.
         * Checks if the key exists in the tree.
         * If the key was found, return true.
         * Otherwise, the rank calculated will match all keys that are less significant
         * than the provided key using the calc_functor, and return false.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(log n).
         */
        template<class FUNCTOR>
        std::shared_ptr<NODE> rank(FUNCTOR calc_functor) const
        {
            SearchPath curr_step = SearchPath::start;
            std::shared_ptr<NODE> current = Avl::tree_root;
            while(curr_step != SearchPath::end)
            {
                curr_step = calc_functor(current);
            }
            return current;
        }
    };
}
#endif