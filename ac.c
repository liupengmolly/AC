#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ac.h"

void node_insert(trie_node *node, line_code *code,int start_index,int index);

char* custom_malloc(char *s,int len){
    s = (char*)malloc(sizeof(char)*(len+1));
    while(s==NULL)s = (char*)malloc(sizeof(char)*(len+1));
    memset(s,0,sizeof(char)*(len+1));
    return s;
}

void custom_ncpy(char *des, char *s, int n) {
	char *p = des;
	while (n > 0 ) {
		*des++ = *s++;
		n--;
	}
	*des = '\0';
	return ;
}

int str2bi(char *str, int len_s, char *bi) {
	int i=0,k = 0;
	while (k<len_s && str[i]!='\n' && str[i]!='\0') {
        if((str[i]&128)==0){
            bi = (char*)realloc(bi,sizeof(char)*((++len_s)*8+1));
            for(int j = 7;j>=0;j--){
                bi[k*8+j] = '0';
            }
            k++;
        }
		for (int j = 7; j >= 0; j--) {
			bi[k * 8 + j] = (str[i] & 1)+'0';
			str[i] >>= 1;
		}
		i++;
        k++;
	}
	bi[k * 8] = '\0';
	return len_s;
}

void str2code(char *str,line_code *code){
	int str_len = strlen(str);
    code->str = custom_malloc(code->str,strlen(str));
    strcpy(code->str,str);
    code->s = custom_malloc(code->s,str_len * 8);
    code->len = str2bi(str,str_len,code->s)*8;
	code->next = NULL;
	return ;
}

int read_file(FILE *f,line_code *last,int line_num){
	int i = 0;
	if (feof(f))return 0;
	char *s=(char*)malloc(sizeof(char)*66);
	while (!feof(f) && i<line_num) {
		i+=1;
		line_code *node = (line_code*)malloc(sizeof(line_code));
		while(node==NULL)node = (line_code*)malloc(sizeof(line_code));

		fgets(s, 66, f);
		str2code(s, node);
		last->next = node;
		last = node;
	}
	free(s);
	s = NULL;
	return 1;
}

void write_code(line_code *code_head) {
	/*
	将字符串链表转化为二进制编码
	*/
	FILE *file = fopen("pattern+string/code100.txt","wb");
	line_code *write_code = code_head->next;
	while(write_code!=NULL){
		fprintf(file, "%s", write_code->s);
		fprintf(file, "\n");
		write_code = write_code->next;
	}
	fclose(file);
	return ;
}

void free_line_code(line_code *code){
    free(code->s);
    code->s = NULL;
    free(code);
    code = NULL;
}

void free_trie_node(trie_node *node){
    free(node->slice);
    node->slice = NULL;
    free(node);
    node = NULL;
}

void init_trienode(trie_node *node,int len,int char_sign,trie_node *parent){
    node->len = len;
    node->num = 0;
    node->char_sign = char_sign;
	node->slice = NULL;
    node->str = NULL;
    node->left = NULL;
    node->right = NULL;
    node->fail = NULL;
    node->parent = parent;
    return ;
}

void allocate_node_by_code(trie_node *parent, line_code *code,int start_index,int index,int same_length){
    /*注意考虑code->len > same_length 的前提*/
    trie_node *new_node = (trie_node*)malloc(sizeof(trie_node));
    int suffix_len = 0;
    if(index+same_length<16)suffix_len = 16-index-same_length;
    else if(index+same_length==16)
        suffix_len = 16;
    init_trienode(new_node,suffix_len,1,parent);
    new_node->slice = (char*)malloc(sizeof(char)*(suffix_len+1));
    custom_ncpy(new_node->slice,code->s+start_index+same_length,suffix_len);
    code->len = code->len - same_length - suffix_len;
    if(new_node->slice[0]=='0')parent->left = new_node;
    else parent->right = new_node;
    
    if(code->len==0)new_node->num = 1;
    else if(code->len>0){
        start_index += (suffix_len+same_length);
        while(code->len>0){
            trie_node *newer_node = (trie_node *)malloc(sizeof(trie_node));
            init_trienode(newer_node,16,1,new_node);
            newer_node->slice = (char*)malloc(sizeof(char)*(16+1));
            custom_ncpy(newer_node->slice,(code->s+start_index),16);
            if(newer_node->slice[0]=='0')new_node->left = newer_node;
            else new_node->right = newer_node;
            code->len -= 16;
            start_index+=16;
            new_node = newer_node;
        }
    }else{
        printf("index value error,smaller than 0\n");
        exit(1);
    }
    new_node->str = custom_malloc(new_node->str,strlen(code->str));
    strcpy(new_node->str,code->str);
    return ;
}

void node_insert(trie_node *node, line_code *code,int start_index,int index){
    //根节点
    if(node->parent == NULL){
        if((node->left!=NULL)&&(code->s+start_index)[0]=='0')
            node_insert(node->left,code,start_index,index);
        else if((node->right!=NULL)&&(code->s+start_index)[0]=='1')
            node_insert(node->right,code,start_index,index);
        else{
            while(code->len>0){
                trie_node *new_node = (trie_node *)malloc(sizeof(trie_node));
                init_trienode(new_node,16,1,node);
                new_node->slice = (char*)malloc(sizeof(char)*(16+1));
                custom_ncpy(new_node->slice,(code->s+start_index),16);
                if(new_node->slice[0]=='0')node->left = new_node;
                else node->right = new_node;
                code->len -= 16;
                start_index+=16;
                node = new_node;
            }
            node->str = custom_malloc(node->str,strlen(code->str));
            strcpy(node->str,code->str);
        }
    }
    //非根节点，且节点与字符串相等
    else if(strcmp(node->slice,(code->s+start_index))==0){
        if(node->str == NULL){
            node->str = custom_malloc(node->str,strlen(code->str));
            strcpy(node->str,code->str);
        }
    }
    //非根节点，节点与字符串不相等
    else{
        char *tmp_slice = node->slice;
        char *tmp_s = (code->s+start_index);
        int same_length = 0;
        while(*tmp_slice!='\0' && *tmp_s!='\0' && *tmp_slice==*tmp_s && index+same_length<16){
            same_length++;
            tmp_slice++;
            tmp_s++;
        }
		//与注释部分的差别在于没有释放掉node去重新产生一个新的trie_suffix,而是直接在现有的node上改变（但其实也没有性能的提升）
		if (same_length < node->len && same_length < code->len) {
            trie_node *new_parent = (trie_node*)malloc(sizeof(trie_node));
            init_trienode(new_parent,same_length,0,node->parent);
            new_parent->slice = (char*)malloc(sizeof(char)*(same_length+1));
            custom_ncpy(new_parent->slice,node->slice,same_length);
            if(node->parent->left == node)node->parent->left = new_parent;
            else node->parent->right = new_parent;

			node->parent = new_parent;
			node->slice = node->slice + same_length;
			node->len = node->len - same_length;
			if (node->slice[0] == '0')new_parent->left = node;
			else new_parent->right = node;

            allocate_node_by_code(new_parent, code, start_index,index,same_length);
		}
		else if(same_length == node->len){
            if(node->left!=NULL && node->left->slice[0]==(code->s+start_index)[same_length]){
                code->len = code->len-same_length;
                node_insert(node->left,code,start_index+same_length,(index+same_length)%16);
            }else if(node->right!=NULL && node->right->slice[0]==(code->s+start_index)[same_length]){
                code->len = code->len-same_length;
                node_insert(node->right,code,start_index+same_length,(index+same_length)%16);
            }else{
                trie_node * new_node = (trie_node*)malloc(sizeof(trie_node));
                int suffix_len = 0;
                if(same_length+index == 16){
                    suffix_len = 16;
                }else
                    suffix_len = 16-same_length-index;
                init_trienode(new_node,suffix_len,1,node);
                new_node->slice = (char*)malloc(sizeof(char)*(suffix_len+1));
                custom_ncpy(new_node->slice,(code->s+start_index)+same_length,suffix_len);
                code->len -= (same_length+suffix_len);

                if(new_node->slice[0]=='0') node->left = new_node;
                else if(new_node->slice[0]=='1')node->right = new_node;
                if(code->len > 0){
                    start_index+=(suffix_len+same_length);
                    while(code->len>0){
                        trie_node *newer_node = (trie_node *)malloc(sizeof(trie_node));
                        init_trienode(newer_node,16,1,new_node);
                        newer_node->slice = (char*)malloc(sizeof(char)*(16+1));
                        custom_ncpy(newer_node->slice,(code->s+start_index),16);
                        if(newer_node->slice[0]=='0')new_node->left = newer_node;
                        else new_node->right = newer_node;
                        code->len -= 16;
                        start_index+=16;
                        new_node = newer_node;
                    }
                }
                new_node->str = custom_malloc(new_node->str,strlen(code->str));
                strcpy(new_node->str,code->str);
            }
        }
        // else if (same_length == code->len){
        //     trie_node *new_parent = (trie_node*)malloc(sizeof(trie_node));
        //     init_trienode(new_parent,same_length,0,node->parent);
        //     new_parent->slice = (char*)malloc(sizeof(char)*(same_length+1));
        //     strcpy(new_parent->slice,(code->s+start_index));
        //     if(node->parent->left == node)node->parent->left = new_parent;
        //     else node->parent->right = new_parent;
		// 	node->parent = new_parent;
		// 	node->slice = node->slice + same_length;
		// 	node->len = node->len - same_length;
		// 	if (node->slice[0] == '0')new_parent->left = node;
		// 	else new_parent->right = node;
        // }
    }
    return ;
}

int node_match(trie_node *node,line_code *code,int start_index){
    if(node!=NULL && node->parent==NULL && code->len!=0){
        if(code->s[0]=='0')return node_match(node->left,code,start_index);
        if(code->s[0]=='1')return node_match(node->right,code,start_index);
    }
    if(node==NULL || node->len==0 || code->len==0)return 0;
    if(strcmp(node->slice,code->s+start_index)==0)return node->num;
    char *tmp_slice = node->slice;
    char *tmp_s = code->s+start_index;
    int same_length = 0;
    while(*tmp_slice!='\0' && *tmp_s!='\0' && *tmp_slice==*tmp_s){
        same_length++;
        tmp_slice++;
        tmp_s++;
    }
    if(same_length<node->len && same_length<code->len)return 0;
    else if(same_length == code->len)return 0;
    else if(same_length == node->len){
        code->len = code->len-same_length;
        if((code->s+start_index+same_length)[0]=='0')return node_match(node->left,code,start_index+same_length);
        if((code->s+start_index+same_length)[0]=='1')return node_match(node->right,code,start_index+same_length);
    }
    return 0;
}

void revstr(char *str, size_t len){
    char *start = str;
    char *end = str + len - 1;
    char ch;
    if (str != NULL)
    {
        while(start < end)
        {
            ch = *start;
            *start++ = *end;
            *end-- = ch;
        }
    }
    return;
}

void print_parent(FILE *file,trie_node *T){
    if(T->slice==NULL)return;
    trie_node *p = T;//指针会改变，所以在有后续操作时，不能直接对指针进行操作，防止后续影响
    char *des;
    des = custom_malloc(des,58*16);
    while(p->slice!=NULL){
        char *tmp;
        tmp = custom_malloc(tmp,strlen(p->slice)+3);
        strcpy(tmp,p->slice);//重新赋值一个变量，防止原变量被改变
        if(p->char_sign == 1){
            tmp[p->len] = ' ';
            tmp[p->len+1] = '/';
        }
        revstr(tmp,strlen(tmp));
        int len_des = strlen(des);
        if(len_des!=0)des[len_des]=' ';
        strcat(des,tmp);
        free(tmp);//防止内存泄漏
        tmp = NULL;//防止野指针
        p=p->parent;
    }
    revstr(des,strlen(des));
    fprintf(file,"%s ",des);
    free(des);
    des = NULL;
    return;
}

int end_flag = 0;
int count = 0;
void print_out(FILE *file,trie_node *T){
    if(T==NULL)return;
    if(T->slice!=NULL){
        fprintf(file,"%s ",T->slice);
        if(T->char_sign==1)fprintf(file,"/ ");
        end_flag = 0;
    }
    print_out(file,T->left);
    if(T->right!=NULL){
        if(end_flag == 1)print_parent(file,T);
        print_out(file,T->right);
    }
    if(T->left==NULL && T->right==NULL){
        end_flag=1;
        if(T->str!=NULL)
            fprintf(file,"%s",T->str);
            fprintf(file,"\n");
    }
    return ;
}

int main(){
	if (access("pattern+string/pattern.txt", 0)) {
		printf("not_exist");
		exit(0);
	}
	FILE *f = NULL;
	f = fopen("pattern+string/new_pattern.txt", "r");
	line_code *code_head = (line_code *)malloc(sizeof(line_code));
	code_head->s = NULL;
	code_head->next = NULL;
	line_code *last = code_head;

    trie_node *T = (trie_node*)malloc(sizeof(trie_node));
	init_trienode(T, 0, 0,NULL);
	int i = 0;
	while (read_file(f, last, 10000)) {
        line_code *head = code_head;
        write_code(head);

		line_code *code = code_head->next;
		while(code!=NULL){
			line_code *p = code;
			node_insert(T,code,0,0);
			code=code->next;
			free(p->s);
			p->s = NULL;
			free(p);
			p = NULL;
		}
		code_head = last;
        break;
	}
	FILE *file = fopen("pattern+string/out.txt","wb");
    print_out(file,T);
    fclose(file);
	return 0;
}
