#include <klibc.h>
#include <fs.h>
file_node_t *root;
void *desc_table;

static file_node_t *alloc_node(void)
{
	file_node_t *node = malloc(sizeof(file_node_t));
	assert(node != NULL);
	memset(node, 0, sizeof(file_node_t));
	list_init(&node->list);
	list_init(&node->children);
	node->hook = malloc(sizeof(file_hook_t));
	assert(node->hook != NULL);
	memset(node->hook, 0, sizeof(file_hook_t));
	return node;
}
static void free_node(file_node_t *node)
{
	if (node->name != NULL)
		free(node->name);
	if (node->hook != NULL)
		free(node->hook);
	free(node);
}
/**
 * Find a node which matches the given path or 
 * a mountpoint node which matches the path's prefix
 */
static file_node_t *find_node(const char *path)
{
	char *path_buf = malloc(strlen(path));
	strcpy(path_buf, path);
	
	char *save_ptr;
	char *dir_name = strtok_r(path_buf, "/", &save_ptr);
	file_node_t *node = root;
	
	while(dir_name != NULL)
	{
		list_head_t *it;
		list_for_each(it, &node->children)
		{
			file_node_t *entry = list_entry(it, file_node_t, list);
			if (strcmp(entry->name, dir_name) == 0)
			{
				node = entry;
				break;
			}
		}
		
		if (strcmp(node->name, dir_name) != 0)
		{
			if (node->type == FILE_TYPE_MOUNT_POINT)
				return node;
			else
				return NULL;
		}
		
		dir_name = strtok_r(NULL, "/", &save_ptr);
	}
	free(path_buf);
	return node;
}
file_node_t *fs_getnode(const char *path)
{
	file_node_t *node = find_node(path);
	if (node == NULL)
		return NULL;
	if(strcmp(path, node->name) == 0)
		return node;
	return NULL;
}
void fs_init(void)
{
	root = alloc_node();
	
	root->name = malloc(2);
	strcpy(root->name,  "/");
	root->type = FILE_TYPE_ROOT;
}
int fs_mknode(const char *path, uint64_t type)
{
	if (strcmp(path, "/") == 0)
		return -1;
	
	char *new_node_name = strrchr(path, '/') + 1;
	
	char *dir_path_buf = malloc(new_node_name - path);
	memcpy(dir_path_buf, path, new_node_name - path);
	dir_path_buf[new_node_name - path - 1] = '\0';
	
	file_node_t *parent_dir = find_node(dir_path_buf);
	if (parent_dir == NULL)
		return -1;
	
	file_node_t *new_node = alloc_node();
	new_node->name = malloc(strlen(new_node_name) + 1);
	strcpy(new_node->name, new_node_name);
	new_node->type = type;
	new_node->parent = parent_dir;
	list_add_before(&parent_dir->children, &new_node->list);
	free(dir_path_buf);
}
int fs_rmnode(const char *path)
{
	if (strcmp(path, "/") == 0)
		return -1;
	
	file_node_t *target = find_node(path);
	if (target == NULL)
		return -1;
	
	if (!list_empty(&target->children))
	{
		return -1;
	}
	
	list_del(&target->list);
	free_node(target);
	return 0;
}
int fs_reghook(const char *path, int hook_index, void *hook)
{
	if (strcmp(path, "/") == 0)
		return -1;
	
	file_node_t *target = find_node(path);
	if (target == NULL)
		return -1;
	
	assert(hook_index < HOOK_MAX);
	*((uint64_t *)target->hook + hook_index) = (uint64_t)hook;
	
	return 0;
}
fs_context_t *fs_open(const char *path,int oflag)
{
	fs_context_t *context = malloc(sizeof(fs_context_t));
	file_node_t *node = fs_getnode(path);
	context->context = node->hook->open(node, path, oflag);
	if(context->context == NULL)
		return -1;
	context->node = node;
	return context;
}
int fs_close(fs_context_t *context)
{
	if (context->node->hook->close == NULL)
		return -1;
	int ret = context->node->hook->close(context->node, context->context);
	free(context->context);
	return ret;
}
int fs_read(fs_context_t *context, void *buf, uint64_t size)
{
	if (context->node->hook->read == NULL)
		return -1;
	return context->node->hook->read(context->node, context->context, buf, size);
}
int fs_write(fs_context_t *context, void *buf, uint64_t size)
{
	if (context->node->hook->write == NULL)
		return -1;
	return context->node->hook->write(context->node, context->context, buf, size);
}
off_t fs_lseek(fs_context_t *context, off_t offset, int whence)
{
	if (context->node->hook->lseek == NULL)
		return -1;
	return context->node->hook->lseek(context->node, context->context, offset, whence);
}
int fs_ioctl(fs_context_t *context, int request)
{
	if (context->node->hook->ioctl == NULL)
		return -1;
	return context->node->hook->ioctl(context->node, context->context, request);
}
int fs_fstat(fs_context_t *context, void *buf)
{
	if (context->node->hook->fstat == NULL)
		return -1;
	return context->node->hook->fstat(context->node, context->context, buf);
}

