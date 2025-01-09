// COP 3502C Assignment 5 
// This program is written by: Kelvin Solorzano

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_LEN 25 
#define MAX_COMMANDS 300000

// Define Node structure
typedef struct Node
{
    char name[MAX_LEN + 1];
    int fine;
    struct Node *left;
    struct Node *right;
    int height;
} Node;

// Function to create a new node
Node* create_node(int value, const char* name)
{
    Node* temp = (Node*) malloc(sizeof(Node));
    if (temp == NULL)
    {
        // Memory allocation failed
        exit(1);
    }

    temp->fine = value;
    strncpy(temp->name, name, MAX_LEN);
    temp->name[MAX_LEN] = '\0'; // Ensure null termination
    temp->left = NULL;
    temp->right = NULL;
    temp->height = 0; // Initialize height to 0 for a new node 
    return temp;
}

// Function to find max value node in subtree 
Node* maxValueNode(Node* node)
{
    Node* current = node;
    while (current && current->right != NULL)
        current = current->right;
    return current;
}

// Function to get height of a node
int get_height(Node* node) {
    if (node == NULL)
        return -1;
    return node->height;
}

// Function to update height of a node
void update_height(Node* node) {
    // Update the node's height based on the height of less and right subtrees
    if (node != NULL) {
        int left_height = get_height(node->left);
        int right_height = get_height(node->right);
        node->height = (left_height > right_height ? left_height : right_height) + 1;
    }
}

// Function to delete a node by name replacing with max node from left subtree
Node* deleteNode(Node* root, const char* name)
{
    // Base Case
    if (root == NULL)
        return root;

    // Compare name with root's name
    int cmp = strcmp(name, root->name);
    if (cmp < 0)
        root->left = deleteNode(root->left, name);
    else if (cmp > 0)
        root->right = deleteNode(root->right, name);
    else
    {
        // Node with only one child or no child
        if (root->left == NULL)
        {
            Node* temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            Node* temp = root->left;
            free(root);
            return temp;
        }

        // Node with two children
        // Get in order predecessor that is max in left subtree
        Node* temp = maxValueNode(root->left);

        // Copy in order predecessor's content to this node
        strcpy(root->name, temp->name);
        root->fine = temp->fine;

        // Delete in order predecessor
        root->left = deleteNode(root->left, temp->name);
    }

    // Update height after deletion
    update_height(root);

    return root;
}

// Function to find depth of a node by name using recursion
int findDepth(Node* root, const char* name) {
    if (root == NULL) {
        // Base Case node not found
        return -1;
    }

    int cmp = strcmp(name, root->name);

    if (cmp == 0) {
        // Target node found
        return 0;
    } else if (cmp < 0) {
        // Target node is in left subtree
        int leftDepth = findDepth(root->left, name);
        if (leftDepth == -1) {
            // Node not found in left subtree
            return -1;
        } else {
            // Add 1 for edge from root to left child
            return leftDepth + 1;
        }
    } else {
        // Target node is in right subtree
        int rightDepth = findDepth(root->right, name);
        if (rightDepth == -1) {
            // Node not found in right subtree
            return -1;
        } else {
            // Add 1 for edge from root to right child
            return rightDepth + 1;
        }
    }
}

// Function to search for a node by name
Node* searchNode(const char* name, Node *root)
{
    if (root == NULL || strcmp(root->name, name) == 0)
    {
        return root;
    }
    // Simple compare variable that will check whether name or root name is bigger lexicographically
    int cmp = strcmp(name, root->name);
    if(cmp > 0)
    {
        return searchNode(name, root->right);
    }
    else
    {
        return searchNode(name, root->left);
    }
}

// Function to calculate sum of fines and count for average 
void calculateSumAndCount(Node* root, long long *sum, int *count)
{
    if (root == NULL) return;

    // Traverse left subtree
    calculateSumAndCount(root->left, sum, count);

    // Accumulate fine and increment count
    *sum += root->fine;
    *count += 1;

    // Traverse right subtree
    calculateSumAndCount(root->right, sum, count);
}

// Helper function to calculate sum of fines for calc_below
void calculateSumBelow(Node* root, const char* name, long long* sum) {
    if (root == NULL)
        return;

    // Compare current node's name with target name
    int cmp = strcmp(root->name, name);

    if (cmp <= 0) {
        // Current node's name is less than or equal to target name lexicographically
        *sum += root->fine;

        // All nodes in left subtree are less than or equal to current node's name lexicographically
        calculateSumBelow(root->left, name, sum);

        // Nodes in right subtree may also be less than or equal to target name lexicographically
        calculateSumBelow(root->right, name, sum);
    } else {
        // Current node's name is greater than target name
        calculateSumBelow(root->left, name, sum);
    }
}

// Function to calculate and display average fine
void average_fine(Node *root)
{
    long long sum = 0;
    int count = 0;

    calculateSumAndCount(root, &sum, &count);

    if (count == 0)
    {
        printf("0.00\n");
        return;
    }

    double ave = (double)sum / count;

    // Truncate to two decimal places
    ave = floor(ave * 100.0) / 100.0;

    printf("%.2lf\n", ave);
}

// Function to check if BST is balanced at root node
void height_balance(Node *root) {
    if (root == NULL) {
        // If tree is empty left and right heights are -1
        printf("left height = -1 right height = -1 balanced\n");
        return;
    }
    
    // Use stored heights for O(1) access
    int lefight = get_height(root->left);
    int righight = get_height(root->right);
    
    // Compare heights
    if (lefight == righight) {
        printf("left height = %d right height = %d balanced\n", lefight, righight);
    } else {
        printf("left height = %d right height = %d not balanced\n", lefight, righight);
    }
}

// Function to deduct fine and possibly delete node
void deduct(Node **root, const char* name, int fine)
{
    Node *person = searchNode(name, *root);
    if(person == NULL)
    {
        printf("%s not found\n", name);
        return;
    }

    if(person->fine <= fine)
    {
        // Deduction leads to removal if fine deducted is bigger than owner's fine
        *root = deleteNode(*root, name);
        printf("%s removed\n", name);
    }
    else
    {
        // Deduct fine and print updated record
        person->fine -= fine;
        int depth = findDepth(*root, name);
        printf("%s %d %d\n", person->name, person->fine, depth);
    }
}

// Function to calculate and display total fines for names less than or equal to given name
void calc_below(const char* name, Node *root)
{
    long long sum = 0;
    calculateSumBelow(root, name, &sum);
    printf("%lld\n", sum);
}

// Function to insert a node into BST or update fine if name exists
Node* add(Node *root, Node *new_node, int *depth)
{
    if (root == NULL)
    {
        *depth = 0;
        return new_node;
    }
    else
    {
        int cmp = strcmp(new_node->name, root->name);
        // Checks if new_node name is lexicographically bigger than root name

        if(cmp > 0)
        {
            // Will also calculate depth of name
            int temp_depth;
            root->right = add(root->right, new_node, &temp_depth);
            if(temp_depth != -1)
                *depth = temp_depth + 1;
        }
        // Checks if new_node name is lexicographically smaller than root name

        else if(cmp < 0)
        {
            // Will also calculate depth of name
            int temp_depth;
            root->left = add(root->left, new_node, &temp_depth);
            if(temp_depth != -1)
                *depth = temp_depth + 1;
        }
        else
        {
            // Duplicate name found update fine
            root->fine += new_node->fine;
            *depth = findDepth(root, root->name);
            free(new_node);
        }
        // Update height after insertion
        update_height(root);
        return root;
    }
}

// Function to handle input commands
void handle_input(Node **root, const char *input, const char *name, int fine)
{
    if(strcmp(input, "add") == 0)
    {
        Node* existing = searchNode(name, *root);
        int depth = 0;
        if(existing != NULL)
        {
            // Vehicle owner exists update fine
            existing->fine += fine;
            depth = findDepth(*root, name);
            printf("%s %d %d\n", existing->name, existing->fine, depth);
        }
        else
        {
            // Vehicle owner does not exist insert new node
            Node* new_node = create_node(fine, name);
            *root = add(*root, new_node, &depth);
            printf("%s %d %d\n", new_node->name, new_node->fine, depth);
        }
    }
    else if(strcmp(input, "deduct") == 0)
    {
        deduct(root, name, fine);
    }
    else if(strcmp(input, "search") ==0)
    {
        Node* found = searchNode(name, *root);
        if(found)
        {
            // prints remaining fine owner has to pay and
            // depth of node in the tree that stores that owner's record
            int depth = findDepth(*root, name);
            printf("%s %d %d\n", found->name, found->fine, depth);
        }
        else
            printf("%s not found\n", name);
    }
    else if(strcmp(input, "calc_below") ==0)
    {
        calc_below(name, *root);
    }
    else if(strcmp(input, "average") ==0)
    {
        average_fine(*root);
    }
    else if(strcmp(input, "height_balance") ==0)
    {
        height_balance(*root);
    }
    else
    {
       // Invalid command, do nothing or handle error
       return;
    }
}

// Function to free all nodes in BST
void free_bst(Node* root)
{
    if (root == NULL) return;
    free_bst(root->left);
    free_bst(root->right);
    free(root);
}

int main()
{
    int N = 0; // Variable for number of commands
    char name[MAX_LEN +1];
    int fine = 0;
    char input[100];

    Node* root = NULL;

    // The following will take input from user and based on the command it will
    // insert different variables in the parameters for the handle_input function
    scanf("%d", &N);
    for(int i = 0; i < N && i < MAX_COMMANDS; i++)
    {
        scanf("%s", input);
        if(strcmp(input, "add") ==0 || strcmp(input, "deduct") == 0)
        {
            scanf("%s %d", name, &fine);
            handle_input(&root, input, name, fine);
        }
        else if(strcmp(input, "search") ==0 || strcmp(input, "calc_below") == 0)
        {
            scanf("%s", name);
            handle_input(&root, input, name, 0);
        }
        else if(strcmp(input, "average") ==0 || strcmp(input, "height_balance") == 0)
        {
            handle_input(&root, input, "", 0);
        }
    }
    // Free BST
    free_bst(root);

    return 0;
}
