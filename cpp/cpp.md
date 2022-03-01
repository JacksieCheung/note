## cpp

### vector

* 内存分配

  vector 会自动为成员分配内存，所以 push_back 方法时，不用用 new 先分配。同理，pop_back 方法也会自动释放内存。

* swap

  有两种方法交换元素

  ```cpp
  swap(vector[i],vector[j]);
  iter_swap(vector.begin() + i,vecotr.end()-1);
  ```

  注意 vector 的 end 方法，指向最后一个元素的下一个元素。

