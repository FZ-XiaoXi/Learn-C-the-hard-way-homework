#include <lcthw/list_algos.h>
#include <lcthw/dbg.h>

inline void ListNode_swap(ListNode *a, ListNode *b)
{
	void *temp;
	temp=a->value;
	a->value=b->value;
	b->value=temp;
}

int List_bubble_sort(List *list, List_compare cmp)
{	
	ListNode *cur,*now;
	if(List_count(list)<=1) return 0;
	for(cur=list->last->prev;cur!=NULL;cur=cur->prev)
	{
		for(now=list->first;;now=now->next)
		{
			if(cmp(now->value,now->next->value)>0) ListNode_swap(now,now->next);
			if(now==cur) break;
		}
		if(cur==list->first) break;
	}
	return 0;
}

List *List_merge(List *left, List *right, List_compare cmp)
{
	
	ListNode *cL,*cR;
	cL=left->first;
	cR=right->first;
	List *sorted=List_create();
	while(1){
		if(cmp(cL->value,cR->value)>0){
			List_push(sorted,cR->value);
			cR=cR->next;
		}else{
			List_push(sorted,cL->value);
			cL=cL->next;
		}
		if(cL==NULL) while(cR!=NULL) {List_push(sorted,cR->value);cR=cR->next;}
		if(cR==NULL) while(cL!=NULL) {List_push(sorted,cL->value);cL=cL->next;}
		if(cL==NULL&&cR==NULL) {
			return sorted;
		}
	}
}

List *List_merge_sort(List *list, List_compare cmp)
{
	int left,right;
	if(List_count(list)<=1){
		return list;	
	}
	if(List_count(list)%2==0){
		left=List_count(list)/2;
		right=left;
	}else{
		left=List_count(list)/2;
		right=List_count(list)/2+1;
	}
	List *leftList=List_create();
	List *rightList=List_create();
	int i=0;
	for(ListNode *cur=list->first;i<left;i++,cur=cur->next) List_push(leftList ,cur->value);
	i=0;
	for(ListNode *cur=list->last;i<right;i++,cur=cur->prev) List_unshift(rightList ,cur->value);
	List *leftList_sorted=List_merge_sort(leftList,cmp);
	List *rightList_sorted=List_merge_sort(rightList,cmp);
	if(leftList_sorted!=leftList) List_destroy(leftList);
	if(rightList_sorted!=rightList) List_destroy(rightList);
	List *sorted=List_merge(leftList_sorted,rightList_sorted,cmp);
	List_destroy(leftList_sorted);
	List_destroy(rightList_sorted);
	return sorted;
}
