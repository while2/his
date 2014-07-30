his
===
> *He* wrote this, and named it *his*.

To use this lib, just include the header files.


## MatrixWrapper & Matrix 
[MatrixWrapper.hpp](ImageProcessing/MatrixWrapper.hpp) 
[Matrix.hpp](ImageProcessing/Matrix.hpp)

__MatrixWrapper__ converts other image format to a united one, and provide a `matrix[y][x]` style interface. 

```c++
cv::Mat1b gray;
his::MatrixWrapper<uchar> gray_wrapper(gray.data, gray.rows, gray.cols, gray.step);
cv::Mat3b image;
his::MatrixWrapper<uchar[3]> image_wrapper(image.data, image.rows, image.cols);
```

It takes a __data__ pointer (top left of the image), __rows__ &__cols__ of the image, the meaning of which are intuitive. Another parameter __step__ controls the length (the unit is the tempalte variable type, not bytes) to skip to jump to the next row. By default it equals to __cols__, once the image was cropped, __cols__ will change while __step__ stay the same.

>Make sure the pointer is available during the scope.

__Matrix__ was derived from __MatrixWrapper__, but mangages its own data. Both of them do shallow copy by default, but a __clone()__ method can be used for deep copy.

## Functional style iterator
[Foreach.hpp](ImageProcessing/Foreach.hpp)
[ForeachPair.hpp](ImageProcessing/ForeachPair.hpp)

A series of iteration functions for __MatrixWrapper__.

__for\_each__ takes a few <b>MatrixWrapper</b>s (images with the same size) and a functor (takes one element for each image), then iterates all images, and feed the functor with elements at the same position in different images.

A rgb to gray example:

```c++
his::foreach(image_wrapper, gray_wrapper, [](const uchar rgb[3], uchar &gray) {
	gray = rgb[0] * 0.299 
		 + rgb[1] * 0.587
		 + rgb[2] * 0.114;
}
```

__for\_each\_pair__ have similar interfaces, but iterates all neighboring pixels, with the functor takes two elements for each image. The pairs appear up-down or left-right. In the following example, `b1` and `b2` are two neighboring pixels in `gray_wrapper`, `b2` is right to or below `b1`.

A grayscale laplacian example:
 
```c++
his::for_each_pair(gray_wrapper, laplace_wrapper,
	[](uchar b1, uchar b2, float &f1, float &f2) {
	f1 += (b1 - b2);
	f2 += (b2 - b1);
});
```
> In the image processing sence, to initialize Graphcut problem or Poisson Equations, neighboring 
>  are often needed, this can make life easier.

This [post](http://while2.github.io/abstraction-of-iterations.html) explains more details.

## Access to 2d-indices in iteration
[IdxMap.hpp](ImageProcessing/IdxMap.hpp)

One of the side effects of these functional style iterators, is that they hide the iteration variables. The solution is to provide an extra __IdxMap__ to the functions, which __*appears*__ like a 2-channels int image, with each element the x-y-position of the pixel. Following is an illustration, not the real fact.

```c++
struct Idx { int x, y; };
typedef Matrix<Idx> IdxMap; 
```

With a tricky implementation, __IdxMap__ does not need actually memories.

To print all neighboring positions:
```c++
his::for_each_pair(his::IdxMap(image_wrapper),
	[](his::Idx idx1, his::Idx idx2)
{
	printf("(%d, %d) <-> (%d, %d)\n", idx1.x, idx1.y, idx2.x, idx2.y);
});
```
> __Idx__ must pass to the functor by value or by const reference.


## Filter
[Fliter.hpp](ImageProcessing/Filter.hpp)

Filter is a kind of image transformation where each output pixel depends on a local region of pixels(typically a weighted sum) from the input image. `his::filter` provides an abstracted interface with which you focus on the filter defination instead of the regular iterative processes.

```c++
template<class Mat1, class Mat2, class Mat3, class AccmFunc, class EvalFunc>
void filter(Mat1 &src, Mat2 &dst, Mat3 &kernel, AccmFunc accm, EvalFunc eval);
```

`src` and `dst` are input and output images, `kernel` is usually a weight map, `accm` and `eval` are two functors(usually lambda expressions) defining the filter proceess. For each output pixel, `accm` iterates over all neighboring pixels in the input image and corresponding kernel elements and generates intermediate results(usually captured by lambda expressions to share with `eval`), depends on which `eval` computes the value of the output pixel. 

A sample of gaussian filter
```c++
float sum_rgb[3] = {0, 0, 0}, sum_w = 0;
his::filter(input_image, output_image, kernel,
	[&](const uchar rgb[3], float w)
{
	for (int c = 0; c < 3; ++c)
		sum_rgb[c] += rgb[c] * w;
	sum_w += w;
},
	[&](uchar rgb[3])
{
	for (int i = 0; i < 3; ++i)
	{
		rgb[i] = uchar(sum_rgb[i] / sum_w);
		sum_rgb[i] = 0;
	}
	sum_w = 0;
});
```
See a full version in function FilterSample at [sample.cpp](https://github.com/while2/his/blob/master/sample.cpp).

This [post](http://while2.github.io/abstraction-of-filter.html) explains more details.

## 'The' semantics: a global variable utility
[The.hpp](Miscellaneous/The.hpp)

In many circumstances we want to access to the single unique instance of a particular type.
For example, with a class named __Thing__, we want to access to the __Thing__, without bothering where to put that instance.
This template class The provide a decent way:

```c++
The<Thing>()->CallFunction();
The<Thing>()->a_field;
```

`The<Thing>()` can convert to an pointer, ```*The<Thing>()``` can get a reference(Works in VC).

With a direct connection with typename and instance, we need merely include a header(Thing.h) wherever the __Thing__ is needed.

`The<Thing,1>()`, `The<Thing,2>` can access to different instances, `The<Thing>` is equal to `The<Thing, 0>`.
>__Thing__ must have a default constructor.