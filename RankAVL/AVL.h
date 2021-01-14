#ifndef _AVL_T
#define _AVL_T
#include <memory>
#include <assert.h>
#include "../Exceptions/Exceptions.h"

namespace DS
{
    //Declaration of a template node struct
    template<typename KEY_TYPE, typename VAL_TYPE>
    struct graph_node
    {
        KEY_TYPE key;
        VAL_TYPE val;
        int height = 0;
        std::shared_ptr<graph_node<KEY_TYPE, VAL_TYPE>> father;
        std::shared_ptr<graph_node<KEY_TYPE, VAL_TYPE>> left;
        std::shared_ptr<graph_node<KEY_TYPE, VAL_TYPE>> right;
        virtual ~graph_node() = default;
    };

    // The class NODE in the template must inherit from graph_node for it to work properly.
    template<typename KEY_TYPE=int, typename VAL_TYPE=int, class NODE=graph_node<KEY_TYPE, VAL_TYPE>> 
    class AVL
    {
        /*********************************/
        /*       Protected Section       */
        /*********************************/
    protected:
        std::shared_ptr<NODE> tree_root;
        std::shared_ptr<NODE> leftmost_node;
        std::shared_ptr<NODE> rightmost_node;
        int node_count;

        /* Private Functions: */
        // Static Functions //

        // Return the higher value out of two integers
        static int max(int a, int b)
        {
            return (a > b)? a : b;
        }
        
        // Return the height of node.
        static int height(const std::shared_ptr<NODE>& node)
        {
            if(node == nullptr)
            {
                return -1;
            }
            return node->height;
        }

        // Helper function that allocates and returns a shared pointer to a new node for the tree.
        static std::shared_ptr<NODE> newNode(const KEY_TYPE& key, const VAL_TYPE& val, const std::shared_ptr<NODE>& father = nullptr)
        {
            std::shared_ptr<NODE> node = std::make_shared<NODE>();
            node->key = key;
            node->val = val;
            node->height = 0;
            node->left = nullptr;
            node->right = nullptr;
            node->father = father;
            return node;
        }

        // Get the balance factor of a node
        static int getBalance(const std::shared_ptr<NODE>& node)
        {
            if(node == nullptr)
            {
                return 0;
            }
            return height(node->left) - height(node->right);
        }

        // Deepcopy src tree into dest tree
        static void deepcopy(std::shared_ptr<NODE>& dest, const std::shared_ptr<NODE>& src)
        {
            if(src == nullptr)
            {
                return;
            }
            // newNode(const KEY_TYPE& key, const VAL_TYPE& val, const std::shared_ptr<NODE>& father = nullptr)
            dest = newNode(src->key, src->val, src->father);
            *dest = *src;
            deepcopy(dest->left, src->left);
            deepcopy(dest->right, src->right);
        }

        /*   Class Private Methods   */
        // General right and left rotations for balanced trees, return the new root of the tree.
        virtual std::shared_ptr<NODE> rotateRight(std::shared_ptr<NODE>& sub_root)
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
            sub_root->height = max(height(sub_root->left), height(sub_root->right)) + 1;
            L_sub->height = max(height(L_sub->left), height(L_sub->right)) + 1;

            // Return the new sub root
            return L_sub;
        }


        virtual std::shared_ptr<NODE> rotateLeft(std::shared_ptr<NODE>& sub_root)
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
            sub_root->height = max(height(sub_root->left), height(sub_root->right)) + 1;
            R_sub->height = max(height(R_sub->left), height(R_sub->right)) + 1;

            // Return the new sub root
            return R_sub;
        }

        // An auxiliary insert method for the class' use.
        virtual std::shared_ptr<NODE> insertAux(const KEY_TYPE& key, const VAL_TYPE& val, std::shared_ptr<NODE>& root)
        {
            assert(root != nullptr);
            // 1. Do a normal BST rotation:
            if(root->key > key)
            {
                if(root->left == nullptr)
                {
                    root->left = newNode(key, val, root);
                    root->height = max(height(root->left), height(root->right)) + 1;
                    node_count++;
                    return root;
                }
                root->left = insertAux(key, val, root->left);
            }
            else if(root->key < key)
            {
                if(root->right == nullptr)
                {
                    root->right = newNode(key, val, root);
                    root->height = max(height(root->left), height(root->right)) + 1;
                    node_count++;
                    return root;
                }
                root->right = insertAux(key, val, root->right);
            }
            else //There already exists a node with the same key, so overwrite it's contents.
            {
                root->val = val;
                return root;
            }

            // 2. Update the height of all the nodes in the BST search.
            root->height = max(height(root->left), height(root->right)) + 1;

            // 3. Verify balance factors, and perform rotations if needed.
            int balance_fact = getBalance(root);
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

        // Finds the lowest (leftmost) node from the given root
        std::shared_ptr<NODE> findLowestNode(const std::shared_ptr<NODE>& root) const
        {
            std::shared_ptr<NODE> left_child = root;
            while(left_child->left != nullptr)
            {
                left_child = left_child->left;
            }
            return left_child;
        }

        // Finds the highest (rightmost) node from the given root
        std::shared_ptr<NODE> findHighestNode(const std::shared_ptr<NODE>& root) const
        {
            std::shared_ptr<NODE> right_child = root;
            while(right_child->right != nullptr)
            {
                right_child = right_child->right;
            }
            return right_child;
        }

        // An auxiliary eraese method for the class' use.
        virtual std::shared_ptr<NODE> eraseAux(std::shared_ptr<NODE>& root, const KEY_TYPE& key)
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
                        node_count--; // Decrement the count of nodes in the tree
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
                        node_count--; // Decrement the count of nodes in the tree
                    }
                }
                // Two children
                else
                {
                    // Find the root's replacement node:
                    std::shared_ptr<NODE> nextMin = findLowestNode(root->right);
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
            // Update heights:
            root->height = max(height(root->right), height(root->left)) + 1;
            // Confirm balance factors:
            int balance_fact = getBalance(root);
            // If the node is unbalanced then we need two rotations according to the 4 cases:
            if (balance_fact > 1)
            {
                if (getBalance(root->left) >= 0) // LL rotation
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
                if (getBalance(root->right) <= 0)  //RR rotation
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

        // An auxiliary function for inOrder.
        template<class FUNCTOR>
        void leftmostInOrderAux(const std::shared_ptr<NODE>& p, int* k, FUNCTOR& func) const
        {
            if(p == NULL || *k == 0) return;

            func(p, k);
            inOrderAux(p->right, k, func);
            leftmostInOrderAux(p->father, k, func);
        }

        // An auxiliary function for inOrder.
        // The functor must decrease k for the number of nodes it has taken care of
        template<class FUNCTOR>
        void inOrderAux(const std::shared_ptr<NODE>& p, int* k, FUNCTOR& func) const
        {
            if(p == NULL || *k == 0) return;

            inOrderAux(p->left, k, func);
            func(p, k);
            inOrderAux(p->right, k, func);
        }

        // An auxiliary function for reverseInOrder.
        template<class FUNCTOR>
        void rightmostReverseInOrderAux(const std::shared_ptr<NODE>& p, int* k, FUNCTOR& func) const
        {
            if(p == NULL || *k == 0) return;
            func(p);
            (*k)--;
            reverseInOrderAux(p->left, k, func);
            rightmostReverseInOrderAux(p->father, k, func);
        }

        // An auxiliary function for reverseInOrder.
        template<class FUNCTOR>
        void reverseInOrderAux(const std::shared_ptr<NODE>& p, int* k, FUNCTOR& func) const
        {
            if(p == NULL || *k == 0) return;

            reverseInOrderAux(p->right, k, func);
            if(*k == 0)
            {
                return;
            }
            func(p);
            (*k)--;
            if(*k == 0)
            {
                return;
            }
            reverseInOrderAux(p->left, k, func);
        }

        // An auxiliary function for preOrder.
        template<class FUNCTOR>
        void preOrderAux(const std::shared_ptr<NODE>& p, int* k, FUNCTOR& func) const
        {
            if(p == NULL || k == 0) return;

            func(p);
            (*k)--;
            preOrderAux(p->left, func);
            preOrderAux(p->right, func);
        }

        // An auxiliary function for postOrder.
        template<class FUNCTOR>
        void postOrderAux(const std::shared_ptr<NODE>& p, int* k, FUNCTOR& func) const
        {
            if(p == NULL || k == 0) return;

            postOrderAux(p->left, func);
            postOrderAux(p->right, func);
            func(p);
            (*k)--;
        }

        void deleteTree(std::shared_ptr<NODE>& root) 
        {
            if (root != nullptr) 
            {
                deleteTree(root->left);
                deleteTree(root->right);
                root = nullptr;
            }
        }

        /**********************************/
        /*         Public Section         */
        /**********************************/
    public:
        /*
         * Constructor: AVL
         * Usage: AVL<KEY_TYPE, VAL_TYPE, NODE> tree(root);
         *        AVL<KEY_TYPE, VAL_TYPE, NODE> tree();
         * ---------------------------------------
         * Create an empty AVL tree, or intialize it with a root using the first syntax.
         * Worst time complexity: O(1)
         * 
         * Possible Exceptions:
         * std::bad_alloc
         */
        explicit AVL(const NODE& root) :
        tree_root(std::make_shared<NODE>(root)), leftmost_node(root), rightmost_node(root), node_count(1) { }
        explicit AVL() : tree_root(nullptr), leftmost_node(nullptr), rightmost_node(nullptr), node_count(0) { }

        AVL(const AVL<KEY_TYPE, VAL_TYPE,NODE>& other) :
        tree_root(std::make_shared<NODE>()), leftmost_node(nullptr), rightmost_node(nullptr), node_count(other.node_count)
        {
            deepcopy(tree_root, other.tree_root);
            leftmost_node = findLowestNode(tree_root);
            rightmost_node = findHighestNode(tree_root);
        }

        AVL& operator=(const AVL<KEY_TYPE, VAL_TYPE,NODE>& other)
        {
            deleteTree(tree_root);

            class CopyTree
            {
            public:
                AVL<KEY_TYPE,VAL_TYPE,NODE>* this_tree;
                CopyTree(AVL<KEY_TYPE, VAL_TYPE, NODE>* tree) : this_tree(tree) { }
                void operator()(const std::shared_ptr<graph_node<KEY_TYPE, VAL_TYPE>>& node, int* k)
                {
                    this_tree->insert(node->key, node->val);
                    (*k)--;
                }
            };

            CopyTree functor(this);
            other.inOrder(functor);
            return *this;
        }

        virtual ~AVL()
        {
            deleteTree(tree_root);
        };

        /*
         * Method: insert
         * Usage: tree.insert(key, val);
         * -----------------------------------
         * Inserts the pair (key, val) to the AVL tree.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(log n).
         * 
         * Possible Exceptions:
         * std::bad_alloc
         */
        virtual void insert(const KEY_TYPE& key, const VAL_TYPE& val)
        {
            if(tree_root == nullptr)
            {
                tree_root = newNode(key, val);
                leftmost_node = rightmost_node = tree_root;
                node_count++;
                return;
            }
            tree_root = insertAux(key, val, tree_root);
            
            assert(leftmost_node);
            assert(rightmost_node);
            if(key < leftmost_node->key)
            {
                leftmost_node = findLowestNode(tree_root);
            }
            if(key > rightmost_node->key)
            {
                rightmost_node = findHighestNode(tree_root);
            }
        }
        
        /*
         * Method: erase
         * Usage: tree.erase(key);
         * -----------------------------------
         * Erases the pair corresponding to 'key' from the AVL tree.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(log n).
         */
        virtual void erase(const KEY_TYPE& key)
        {
            tree_root = eraseAux(tree_root, key);
            if(tree_root)
            {
                assert(leftmost_node);
                assert(rightmost_node);
                leftmost_node = findLowestNode(tree_root);
                rightmost_node = findHighestNode(tree_root);
            }
            else
            {
                leftmost_node = nullptr;
                rightmost_node = nullptr;
            }
        }

        /*
         * Method: inOrder
         * Usage: tree.inOrder(functor);
         *        tree.inOrder(functor, k);
         * -----------------------------------
         * Applies the functor for k nodes in an in-order traversal.
         * Input a negative number for k or do not input it to get a full tree traversal.
         * For proper use of this method, do not attempt to change the nodes of the tree.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(n * O(func)).
         * The amortized time complexity is O(k * O(func)).
         */
        template<class FUNCTOR>
        void inOrder(FUNCTOR& func, int k = -1) const
        {
            if(k < 0)
            {
                k = node_count;
            }
            leftmostInOrderAux(leftmost_node, &k, func);
        }

        /*
         * Method: reverseInOrder
         * Usage: tree.reverseInOrder(functor);
         *        tree.reverseInOrder(functor, k);
         * -----------------------------------
         * Applies the functor for k nodes in a reverse in-order traversal.
         * Input a negative number for k or do not input it to get a full tree traversal.
         * For proper use of this method, do not attempt to change the nodes of the tree.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(n * O(func)).
         * The amortized time complexity is O(k * O(func)).
         */
        template<class FUNCTOR>
        void reverseInOrder(FUNCTOR& func, int k = -1) const
        {
            if(k < 0)
            {
                k = node_count;
            }
            rightmostReverseInOrderAux(rightmost_node, &k, func);
        }

        /*
         * Method: preOrder
         * Usage: tree.prOrder(functor);
         *        tree.prOrder(functor, k);
         * -----------------------------------
         * Applies the functor for k nodes in an pre-order traversal.
         * Input a negative number for k or do not input it to get a full tree traversal.
         * For proper use of this method, do not attempt to change the nodes of the tree.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(n * O(func)).
         */
        template<class FUNCTOR>
        void preOrder(FUNCTOR& func, int k = -1) const
        {
            if(k < 0)
            {
                k = node_count;
            }
            preOrderAux(tree_root, &k, func);
        }

        /*
         * Method: postOrder
         * Usage: tree.postOrder(functor);
         * -----------------------------------
         * Applies the functor for k nodes in an post-order traversal.
         * Input a negative number for k or do not input it to get a full tree traversal.
         * For proper use of this method, do not attempt to change the nodes of the tree.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(n * O(func)).
         */
        template<class FUNCTOR>
        void postOrder(FUNCTOR& func, int k = -1) const
        {
            if(k < 0)
            {
                k = node_count;
            }
            postOrderAux(tree_root, &k, func);
        }

        /*
         * Method: getNode
         * Usage: tree.getNode(key);
         * -----------------------------------
         * Finds the key and returns the node it is stored in as a const pointer.
         * If the key wasn't found, throws a KeyNotFound exception.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(log n).
         * 
         * Possible Exceptions (in scope DS::AVL):
         * KeyNotFound.
         */
        const std::shared_ptr<NODE> getNode(const KEY_TYPE& key)
        {
            std::shared_ptr<NODE> node = tree_root;
            if(!node)
            {
                throw KeyNotFound();
            }
            while(node->key != key)
            {
                if(key > node->key)
                {
                    if(node->right == nullptr)
                    {
                        throw KeyNotFound();
                    }
                    node = node->right;
                    continue;
                }
                else if(key < node->key)
                {
                    if(node->left == nullptr)
                    {
                        throw KeyNotFound();
                    }
                    node = node->left;
                    continue;
                }
            }
            // If we reached this point, node now points to the correct address.
            return node;
        }

        /*
         * Method: at
         * Usage: tree.at(key);
         * -----------------------------------
         * Finds the key and returns the value it is mapped to.
         * If the key wasn't found, throws a KeyNotFound exception.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(log n).
         * 
         * Possible Exceptions (in scope DS::AVL):
         * KeyNotFound.
         */
        VAL_TYPE& at(KEY_TYPE& key)
        {
            return getNode(key)->val;
        }

        const VAL_TYPE& at(const KEY_TYPE& key) const
        {
            return getNode(key)->val;
        }

        /*
         * Method: find
         * Usage: tree.find(key);
         * -----------------------------------
         * Checks if the key exists in the tree.
         * If the key was found, return true. Return false otherwise.
         * When n is the total number of keys in the tree, the
         * worst time and space complexity for this method is O(log n).
         */
        bool find(const KEY_TYPE& key) const noexcept
        {
            std::shared_ptr<NODE> node = tree_root;
            if(!node)
            {
                return false;
            }
            while(node->key != key)
            {
                if(key > node->key)
                {
                    if(node->right == nullptr)
                    {
                        return false;
                    }
                    node = node->right;
                    continue;
                }
                else if(key < node->key)
                {
                    if(node->left == nullptr)
                    {
                        return false;
                    }
                    node = node->left;
                    continue;
                }
            }
            // If we reached this point, node now points to the correct address.
            return true;
        }

        /*
         * Method: getLowest
         * Usage: tree.getLowest();
         * -----------------------------------
         * Returns the node of the lowest key in the tree,
         * In the case that the tree is empty, return a null pointer.
         * 
         * The worst time and space complexity for this method is O(1).
         */
        const std::shared_ptr<NODE>& getLowest() const
        {
            return leftmost_node;
        }

        /*
         * Method: getHighest
         * Usage: tree.getHighest();
         * -----------------------------------
         * Returns the node of the highest key in the tree,
         * In the case that the tree is empty, return a null pointer.
         * 
         * The worst time and space complexity for this method is O(1).
         */
        const std::shared_ptr<NODE>& getHighest() const
        {
            return rightmost_node;
        }

        /*
         * Method: size
         * Usage: tree.size();
         * -----------------------------------
         * Returns the number of nodes in the tree,
         * 
         * The worst time and space complexity for this method is O(1).
         */
        int size() const
        {
            return node_count;
        }
    };
}
#endif