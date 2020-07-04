#ifndef COLOUR_H
#define COLOUR_H

namespace venk
{

	class ColorRGBA : public Tuple<float, 4>
	{
	public:
		static const int R = 0;
		static const int G = 1;
		static const int B = 2;
		static const int A = 3;

		static const std::size_t tuple_size = 4;
		static const eScalarType element_type = BufferElementType<float>::element_type;

		ColorRGBA()
		{
		}

		ColorRGBA(float r, float g, float b, float a = 0.0f)
		{
			elements[R] = r;
			elements[G] = g;
			elements[B] = b;
			elements[A] = a;
		}
	};

	namespace Colors
	{
		static auto aliceblue = ColorRGBA(0.941f, 0.973f, 1.000f);
		static auto antiquewhite = ColorRGBA(0.980f, 0.922f, 0.843f);
		static auto aqua = ColorRGBA(0.000f, 1.000f, 1.000f);
		static auto aquamarine = ColorRGBA(0.498f, 1.000f, 0.831f);
		static auto azure = ColorRGBA(0.941f, 1.000f, 1.000f);
		static auto beige = ColorRGBA(0.961f, 0.961f, 0.863f);
		static auto bisque = ColorRGBA(1.000f, 0.894f, 0.769f);
		static auto black = ColorRGBA(0.000f, 0.000f, 0.000f);
		static auto blanchedalmond = ColorRGBA(1.000f, 0.922f, 0.804f);
		static auto blue = ColorRGBA(0.000f, 0.000f, 1.000f);
		static auto blueviolet = ColorRGBA(0.541f, 0.169f, 0.886f);
		static auto brown = ColorRGBA(0.647f, 0.165f, 0.165f);
		static auto burlywood = ColorRGBA(0.871f, 0.722f, 0.529f);
		static auto cadetblue = ColorRGBA(0.373f, 0.620f, 0.627f);
		static auto chartreuse = ColorRGBA(0.498f, 1.000f, 0.000f);
		static auto chocolate = ColorRGBA(0.824f, 0.412f, 0.118f);
		static auto coral = ColorRGBA(1.000f, 0.498f, 0.314f);
		static auto cornflowerblue = ColorRGBA(0.392f, 0.584f, 0.929f);
		static auto cornsilk = ColorRGBA(1.000f, 0.973f, 0.863f);
		static auto crimson = ColorRGBA(0.863f, 0.078f, 0.235f);
		static auto cyan = ColorRGBA(0.000f, 1.000f, 1.000f);
		static auto darkblue = ColorRGBA(0.000f, 0.000f, 0.545f);
		static auto darkcyan = ColorRGBA(0.000f, 0.545f, 0.545f);
		static auto darkgoldenrod = ColorRGBA(0.722f, 0.525f, 0.043f);
		static auto darkgray = ColorRGBA(0.663f, 0.663f, 0.663f);
		static auto darkgreen = ColorRGBA(0.000f, 0.392f, 0.000f);
		static auto darkgrey = ColorRGBA(0.663f, 0.663f, 0.663f);
		static auto darkkhaki = ColorRGBA(0.741f, 0.718f, 0.420f);
		static auto darkmagenta = ColorRGBA(0.545f, 0.000f, 0.545f);
		static auto darkolivegreen = ColorRGBA(0.333f, 0.420f, 0.184f);
		static auto darkorange = ColorRGBA(1.000f, 0.549f, 0.000f);
		static auto darkorchid = ColorRGBA(0.600f, 0.196f, 0.800f);
		static auto darkred = ColorRGBA(0.545f, 0.000f, 0.000f);
		static auto darksalmon = ColorRGBA(0.914f, 0.588f, 0.478f);
		static auto darkseagreen = ColorRGBA(0.561f, 0.737f, 0.561f);
		static auto darkslateblue = ColorRGBA(0.282f, 0.239f, 0.545f);
		static auto darkslategray = ColorRGBA(0.184f, 0.310f, 0.310f);
		static auto darkslategrey = ColorRGBA(0.184f, 0.310f, 0.310f);
		static auto darkturquoise = ColorRGBA(0.000f, 0.808f, 0.820f);
		static auto darkviolet = ColorRGBA(0.580f, 0.000f, 0.827f);
		static auto deeppink = ColorRGBA(1.000f, 0.078f, 0.576f);
		static auto deepskyblue = ColorRGBA(0.000f, 0.749f, 1.000f);
		static auto dimgray = ColorRGBA(0.412f, 0.412f, 0.412f);
		static auto dimgrey = ColorRGBA(0.412f, 0.412f, 0.412f);
		static auto dodgerblue = ColorRGBA(0.118f, 0.565f, 1.000f);
		static auto firebrick = ColorRGBA(0.698f, 0.133f, 0.133f);
		static auto floralwhite = ColorRGBA(1.000f, 0.980f, 0.941f);
		static auto forestgreen = ColorRGBA(0.133f, 0.545f, 0.133f);
		static auto fuchsia = ColorRGBA(1.000f, 0.000f, 1.000f);
		static auto gainsboro = ColorRGBA(0.863f, 0.863f, 0.863f);
		static auto ghostwhite = ColorRGBA(0.973f, 0.973f, 1.000f);
		static auto gold = ColorRGBA(1.000f, 0.843f, 0.000f);
		static auto goldenrod = ColorRGBA(0.855f, 0.647f, 0.125f);
		static auto gray = ColorRGBA(0.502f, 0.502f, 0.502f);
		static auto green = ColorRGBA(0.000f, 0.502f, 0.000f);
		static auto greenyellow = ColorRGBA(0.678f, 1.000f, 0.184f);
		static auto grey = ColorRGBA(0.502f, 0.502f, 0.502f);
		static auto honeydew = ColorRGBA(0.941f, 1.000f, 0.941f);
		static auto hotpink = ColorRGBA(1.000f, 0.412f, 0.706f);
		static auto indianred = ColorRGBA(0.804f, 0.361f, 0.361f);
		static auto indigo = ColorRGBA(0.294f, 0.000f, 0.510f);
		static auto ivory = ColorRGBA(1.000f, 1.000f, 0.941f);
		static auto khaki = ColorRGBA(0.941f, 0.902f, 0.549f);
		static auto lavender = ColorRGBA(0.902f, 0.902f, 0.980f);
		static auto lavenderblush = ColorRGBA(1.000f, 0.941f, 0.961f);
		static auto lawngreen = ColorRGBA(0.486f, 0.988f, 0.000f);
		static auto lemonchiffon = ColorRGBA(1.000f, 0.980f, 0.804f);
		static auto lightblue = ColorRGBA(0.678f, 0.847f, 0.902f);
		static auto lightcoral = ColorRGBA(0.941f, 0.502f, 0.502f);
		static auto lightcyan = ColorRGBA(0.878f, 1.000f, 1.000f);
		static auto lightgoldenrodyellow = ColorRGBA(0.980f, 0.980f, 0.824f);
		static auto lightgray = ColorRGBA(0.827f, 0.827f, 0.827f);
		static auto lightgreen = ColorRGBA(0.565f, 0.933f, 0.565f);
		static auto lightgrey = ColorRGBA(0.827f, 0.827f, 0.827f);
		static auto lightpink = ColorRGBA(1.000f, 0.714f, 0.757f);
		static auto lightsalmon = ColorRGBA(1.000f, 0.627f, 0.478f);
		static auto lightseagreen = ColorRGBA(0.125f, 0.698f, 0.667f);
		static auto lightskyblue = ColorRGBA(0.529f, 0.808f, 0.980f);
		static auto lightslategray = ColorRGBA(0.467f, 0.533f, 0.600f);
		static auto lightslategrey = ColorRGBA(0.467f, 0.533f, 0.600f);
		static auto lightsteelblue = ColorRGBA(0.690f, 0.769f, 0.871f);
		static auto lightyellow = ColorRGBA(1.000f, 1.000f, 0.878f);
		static auto lime = ColorRGBA(0.000f, 1.000f, 0.000f);
		static auto limegreen = ColorRGBA(0.196f, 0.804f, 0.196f);
		static auto linen = ColorRGBA(0.980f, 0.941f, 0.902f);
		static auto magenta = ColorRGBA(1.000f, 0.000f, 1.000f);
		static auto maroon = ColorRGBA(0.502f, 0.000f, 0.000f);
		static auto mediumaquamarine = ColorRGBA(0.400f, 0.804f, 0.667f);
		static auto mediumblue = ColorRGBA(0.000f, 0.000f, 0.804f);
		static auto mediumorchid = ColorRGBA(0.729f, 0.333f, 0.827f);
		static auto mediumpurple = ColorRGBA(0.576f, 0.439f, 0.859f);
		static auto mediumseagreen = ColorRGBA(0.235f, 0.702f, 0.443f);
		static auto mediumslateblue = ColorRGBA(0.482f, 0.408f, 0.933f);
		static auto mediumspringgreen = ColorRGBA(0.000f, 0.980f, 0.604f);
		static auto mediumturquoise = ColorRGBA(0.282f, 0.820f, 0.800f);
		static auto mediumvioletred = ColorRGBA(0.780f, 0.082f, 0.522f);
		static auto midnightblue = ColorRGBA(0.098f, 0.098f, 0.439f);
		static auto mintcream = ColorRGBA(0.961f, 1.000f, 0.980f);
		static auto mistyrose = ColorRGBA(1.000f, 0.894f, 0.882f);
		static auto moccasin = ColorRGBA(1.000f, 0.894f, 0.710f);
		static auto navajowhite = ColorRGBA(1.000f, 0.871f, 0.678f);
		static auto navy = ColorRGBA(0.000f, 0.000f, 0.502f);
		static auto oldlace = ColorRGBA(0.992f, 0.961f, 0.902f);
		static auto olive = ColorRGBA(0.502f, 0.502f, 0.000f);
		static auto olivedrab = ColorRGBA(0.420f, 0.557f, 0.137f);
		static auto orange = ColorRGBA(1.000f, 0.647f, 0.000f);
		static auto orangered = ColorRGBA(1.000f, 0.271f, 0.000f);
		static auto orchid = ColorRGBA(0.855f, 0.439f, 0.839f);
		static auto palegoldenrod = ColorRGBA(0.933f, 0.910f, 0.667f);
		static auto palegreen = ColorRGBA(0.596f, 0.984f, 0.596f);
		static auto paleturquoise = ColorRGBA(0.686f, 0.933f, 0.933f);
		static auto palevioletred = ColorRGBA(0.859f, 0.439f, 0.576f);
		static auto papayawhip = ColorRGBA(1.000f, 0.937f, 0.835f);
		static auto peachpuff = ColorRGBA(1.000f, 0.855f, 0.725f);
		static auto peru = ColorRGBA(0.804f, 0.522f, 0.247f);
		static auto pink = ColorRGBA(1.000f, 0.753f, 0.796f);
		static auto plum = ColorRGBA(0.867f, 0.627f, 0.867f);
		static auto powderblue = ColorRGBA(0.690f, 0.878f, 0.902f);
		static auto purple = ColorRGBA(0.502f, 0.000f, 0.502f);
		static auto red = ColorRGBA(1.000f, 0.000f, 0.000f);
		static auto rosybrown = ColorRGBA(0.737f, 0.561f, 0.561f);
		static auto royalblue = ColorRGBA(0.255f, 0.412f, 0.882f);
		static auto saddlebrown = ColorRGBA(0.545f, 0.271f, 0.075f);
		static auto salmon = ColorRGBA(0.980f, 0.502f, 0.447f);
		static auto sandybrown = ColorRGBA(0.957f, 0.643f, 0.376f);
		static auto seagreen = ColorRGBA(0.180f, 0.545f, 0.341f);
		static auto seashell = ColorRGBA(1.000f, 0.961f, 0.933f);
		static auto sienna = ColorRGBA(0.627f, 0.322f, 0.176f);
		static auto silver = ColorRGBA(0.753f, 0.753f, 0.753f);
		static auto skyblue = ColorRGBA(0.529f, 0.808f, 0.922f);
		static auto slateblue = ColorRGBA(0.416f, 0.353f, 0.804f);
		static auto slategray = ColorRGBA(0.439f, 0.502f, 0.565f);
		static auto snow = ColorRGBA(1.000f, 0.980f, 0.980f);
		static auto springgreen = ColorRGBA(0.000f, 1.000f, 0.498f);
		static auto steelblue = ColorRGBA(0.275f, 0.510f, 0.706f);
		static auto tan = ColorRGBA(0.824f, 0.706f, 0.549f);
		static auto teal = ColorRGBA(0.000f, 0.502f, 0.502f);
		static auto thistle = ColorRGBA(0.847f, 0.749f, 0.847f);
		static auto tomato = ColorRGBA(1.000f, 0.388f, 0.278f);
		static auto turquoise = ColorRGBA(0.251f, 0.878f, 0.816f);
		static auto violet = ColorRGBA(0.933f, 0.510f, 0.933f);
		static auto wheat = ColorRGBA(0.961f, 0.871f, 0.702f);
		static auto white = ColorRGBA(1.000f, 1.000f, 1.000f);
		static auto whitesmoke = ColorRGBA(0.961f, 0.961f, 0.961f);
		static auto yellow = ColorRGBA(1.000f, 1.000f, 0.000f);
		static auto yellowgreen = ColorRGBA(0.604f, 0.804f, 0.196f);

	} // namespace Colors

} // namespace venk
#endif
