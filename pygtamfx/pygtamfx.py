import ctypes as _ctypes
import shutil as _shutil
import os.path as _path
import enum as _enum
import typing

try:
    import glm as glm
except ImportError:
    print("Please install PyGLM.")
    exit(1)

if typing.TYPE_CHECKING:
    _Ptr = _ctypes._Pointer
else:

    class _Ptr:
        def __class_getitem__(self, *_):
            return None


class _CVec2(_ctypes.Structure):
    _fields_ = [("x", _ctypes.c_float), ("y", _ctypes.c_float)]

    def to_glm(self) -> glm.vec2:
        return glm.vec2(self.x, self.y)  # type: ignore

    def set_from_glm(self, v: glm.vec2):
        self.x = _ctypes.c_float(v.x)
        self.y = _ctypes.c_float(v.y)


class _CVec2i(_ctypes.Structure):
    _fields_ = [("x", _ctypes.c_int), ("y", _ctypes.c_int)]

    def to_glm(self) -> glm.ivec2:
        return glm.ivec2(self.x, self.y)  # type: ignore

    def set_from_glm(self, v: glm.ivec2):
        self.x = _ctypes.c_int(v.x)
        self.y = _ctypes.c_int(v.y)


class _CVec3(_ctypes.Structure):
    _fields_ = [("x", _ctypes.c_float), ("y", _ctypes.c_float), ("z", _ctypes.c_float)]

    def to_glm(self) -> glm.vec3:
        return glm.vec3(self.x, self.y, self.z)  # type: ignore

    def set_from_glm(self, v: glm.vec3):
        self.x = _ctypes.c_float(v.x)
        self.y = _ctypes.c_float(v.y)
        self.z = _ctypes.c_float(v.z)


class _CVec4(_ctypes.Structure):
    _fields_ = [
        ("x", _ctypes.c_float),
        ("y", _ctypes.c_float),
        ("z", _ctypes.c_float),
        ("w", _ctypes.c_float),
    ]

    def to_glm(self) -> glm.vec4:
        return glm.vec4(self.x, self.y, self.w)  # type: ignore

    def set_from_glm(self, v: glm.vec4):
        self.x = _ctypes.c_float(v.x)
        self.y = _ctypes.c_float(v.y)
        self.z = _ctypes.c_float(v.z)
        self.w = _ctypes.c_float(v.w)


class _CQuat(_ctypes.Structure):
    _fields_ = [
        ("x", _ctypes.c_float),
        ("y", _ctypes.c_float),
        ("z", _ctypes.c_float),
        ("w", _ctypes.c_float),
    ]

    def to_glm(self) -> glm.quat:
        return glm.quat(self.w, self.x, self.y, self.z)  # type: ignore

    def set_from_glm(self, v: glm.quat):
        self.w = _ctypes.c_float(v.w)
        self.x = _ctypes.c_float(v.x)
        self.y = _ctypes.c_float(v.y)
        self.z = _ctypes.c_float(v.z)


_CWindow = _ctypes.c_void_p


class _CTexture(_ctypes.Structure):
    _fields_ = [("id", _ctypes.c_uint), ("size", _CVec2)]


class _CShader(_ctypes.Structure):
    _fields_ = [("id", _ctypes.c_uint),
                ("vertexCount", _ctypes.c_size_t),
                ("line", _ctypes.c_bool)]


class _CTextureView(_ctypes.Structure):
    _fields_ = [
        ("position", _CVec2),
        ("scale", _CVec2),
        ("source", _ctypes.POINTER(_CTexture)),
    ]


class _CSprite(_ctypes.Structure):
    _fields_ = [
        ("texture", _CTextureView),
        ("shader", _ctypes.POINTER(_CShader)),
        ("color", _CVec4),
        ("position", _CVec3),
        ("scale", _CVec3),
        ("rotation", _CQuat),
    ]


class _CCameraOpts_Perspective_(_ctypes.Structure):
    _fields_ = [
        ("fov", _ctypes.c_float),
        ("aspect", _ctypes.c_float),
        ("zNear", _ctypes.c_float),
        ("zFar", _ctypes.c_float),
    ]


class _CCameraOpts_Orthographic_(_ctypes.Structure):
    _fields_ = [
        ("size", _CVec2),
        ("left", _ctypes.c_float),
        ("right", _ctypes.c_float),
        ("bottom", _ctypes.c_float),
        ("top", _ctypes.c_float),
    ]


class _CCameraOpts_(_ctypes.Union):
    _fields_ = [
        ("perspective", _CCameraOpts_Perspective_),
        ("orthographic", _CCameraOpts_Orthographic_),
    ]


class _CCamera(_ctypes.Structure):
    _fields_ = [
        ("type", _ctypes.c_int),
        ("position", _CVec3),
        ("rotation", _CQuat),
        ("opts", _CCameraOpts_),
    ]


try:
    _C = _ctypes.CDLL("libgtamfx.so")
except OSError:
    _C = _ctypes.CDLL("/usr/local/lib/libgtamfx.so")

_GTAM_ERROR_NONE = 0
_GTAM_ERROR_GLFW_FAILED_INIT = 1
_GTAM_ERROR_GLFW_FAILED_CREATE_WINDOW = 2
_GTAM_ERROR_GL3W_FAILED_INIT = 3
_GTAM_ERROR_GL3W_BAD_VERSION = 4
_GTAM_ERROR_TEXTURE_LOAD_FAIL = 5
_GTAM_ERROR_SHADER_LOAD_FAIL = 6

_GTAM_ERROR_STRINGS = [
    "None",
    "Failed to initialize GLFW",
    "Failed to create window",
    "Failed to initialize GL3W",
    "Bad version reported by GL3W",
    "Failed to load texture",
    "Failed to load shader",
]


def _gtam_error_to_text(err: int) -> str:
    if 0 <= err < len(_GTAM_ERROR_STRINGS):
        return _GTAM_ERROR_STRINGS[err]
    return "Unknown error"


_C.gtamGetError.argtypes = []
_C.gtamGetError.restype = _ctypes.c_int
_C.gtamGetErrorMessage.argtypes = []
_C.gtamGetErrorMessage.restype = _ctypes.c_char_p
_C.gtamCreateWindow.argtypes = [_CVec2i, _ctypes.c_char_p]
_C.gtamCreateWindow.restype = _CWindow
_C.gtamDestroyWindow.argtypes = [_CWindow]
_C.gtamInitWindow.argtypes = [_CWindow]
_C.gtamWindowShouldClose.argtypes = [_CWindow]
_C.gtamWindowShouldClose.restype = _ctypes.c_int
_C.gtamCloseWindow.argtypes = [_CWindow]
_C.gtamUncloseWindow.argtypes = [_CWindow]
_C.gtamUpdateWindow.argtypes = [_CWindow, _ctypes.c_int]
_C.gtamDeinitWindow.argtypes = [_CWindow]
_C.gtamWindowGetTime.argtypes = [_CWindow]
_C.gtamWindowGetTime.restype = _ctypes.c_float
_C.gtamWindowIsKeyDown.argtypes = [_CWindow, _ctypes.c_int]
_C.gtamWindowIsKeyDown.restype = _ctypes.c_int
_C.gtamWindowIsMouseDown.argtypes = [_CWindow, _ctypes.c_int]
_C.gtamWindowIsMouseDown.restype = _ctypes.c_int
_C.gtamWindowGetMousePosition.argtypes = [_CWindow, _ctypes.POINTER(_CVec2)]
_C.gtamWindowNewTexture.argtypes = [_CWindow, _ctypes.c_char_p]
_C.gtamWindowNewTexture.restype = _ctypes.POINTER(_CTexture)
_C.gtamWindowDelTexture.argtypes = [_CWindow, _ctypes.POINTER(_CTexture)]
_C.gtamWindowNewShader.argtypes = [
    _CWindow,
    _ctypes.c_char_p,
    _ctypes.c_char_p,
    _ctypes.c_size_t,
]
_C.gtamWindowNewShader.restype = _ctypes.POINTER(_CShader)
_C.gtamWindowDelShader.argtypes = [_CWindow, _ctypes.POINTER(_CShader)]
_C.gtamWindowNewSprite.argtypes = [
    _CWindow,
    _ctypes.POINTER(_CTexture),
    _ctypes.POINTER(_CShader),
]
_C.gtamWindowNewSprite.restype = _ctypes.POINTER(_CSprite)
_C.gtamWindowDelSprite.argtypes = [_CWindow, _ctypes.POINTER(_CSprite)]
_C.gtamWindowNewCamera.argtypes = [_CWindow, _ctypes.c_int]
_C.gtamWindowNewCamera.restype = _ctypes.POINTER(_CCamera)
_C.gtamWindowDelCamera.argtypes = [_CWindow, _ctypes.POINTER(_CCamera)]
_C.gtamWindowSetActiveCamera.argtypes = [_CWindow, _ctypes.POINTER(_CCamera)]
_C.gtamWindowGetActiveCamera.argtypes = [_CWindow]
_C.gtamWindowGetActiveCamera.restype = _ctypes.POINTER(_CCamera)
_C.gtamWindowGetFramebufferSize.argtypes = [_CWindow, _ctypes.POINTER(_CVec2)]
_C.gtamWindowGetAspectRatio.argtypes = [_CWindow]
_C.gtamWindowGetAspectRatio.restype = _ctypes.c_float


class Texture:
    def __init__(self, handle: _Ptr[_CTexture]):
        self._handle = handle

    @property
    def id(self) -> int:
        return self._handle[0].id

    @property
    def size(self):
        return self._handle[0].size.to_glm()


class Shader:
    def __init__(self, handle: _Ptr[_CShader]):
        self._handle = handle

    @property
    def id(self) -> int:
        return self._handle[0].id

    @property
    def vertexCount(self) -> int:
        return self._handle[0].vertexCount

    @vertexCount.setter
    def vertexCount(self, value: int):
        self._handle[0].vertexCount = value
    
    @property
    def line(self) -> bool:
        return not not self._handle[0].line

    @line.setter
    def line(self, value: bool):
        self._handle[0].line = value


class TextureView:
    def __init__(self, handle: _CTextureView):
        self._handle = handle

    @property
    def position(self) -> glm.vec2:
        return self._handle.position.to_glm()

    @position.setter
    def position(self, value: glm.vec2):
        self._handle.position.set_from_glm(value)

    @property
    def scale(self) -> glm.vec2:
        return self._handle.scale.to_glm()

    @scale.setter
    def scale(self, value: glm.vec2):
        self._handle.scale.set_from_glm(value)

    @property
    def source(self):
        return Texture(self._handle.source)

    @source.setter
    def source(self, value: Texture):
        self._handle.source = value._handle


class Sprite:
    def __init__(self, handle: _Ptr[_CSprite]):
        self._handle = handle

    @property
    def texture(self) -> TextureView:
        return TextureView(self._handle[0].texture)

    @texture.setter
    def texture(self, value: TextureView):
        self._handle[0].texture = value._handle

    @property
    def shader(self) -> Shader:
        return Shader(self._handle[0].shader)

    @shader.setter
    def shader(self, value: Shader):
        self._handle[0].shader = value._handle

    @property
    def color(self) -> glm.vec4:
        return self._handle[0].color.to_glm()

    @color.setter
    def color(self, value: glm.vec4):
        self._handle[0].color.set_from_glm(value)

    @property
    def position(self) -> glm.vec3:
        return self._handle[0].position.to_glm()

    @position.setter
    def position(self, value: glm.vec3):
        self._handle[0].position.set_from_glm(value)

    @property
    def scale(self) -> glm.vec3:
        return self._handle[0].scale.to_glm()

    @scale.setter
    def scale(self, value: glm.vec3):
        self._handle[0].scale.set_from_glm(value)

    @property
    def rotation(self) -> glm.quat:
        return self._handle[0].rotation.to_glm()

    @rotation.setter
    def rotation(self, value: glm.quat):
        self._handle[0].rotation.set_from_glm(value)


class CameraType(_enum.IntEnum):
    UNKNOWN = -1
    ORTHOGRAPHIC = 0
    PERSPECTIVE = 1


class PerspectiveCamera_:
    def __init__(self, handle: _CCameraOpts_Perspective_):
        self._handle = handle

    @property
    def fov(self) -> float:
        return self._handle.fov

    @fov.setter
    def fov(self, v: float):
        self._handle.fov = _ctypes.c_float(v)

    @property
    def aspect(self) -> float:
        return self._handle.fov

    @aspect.setter
    def aspect(self, v: float):
        self._handle.aspect = _ctypes.c_float(v)

    @property
    def z_near(self) -> float:
        return self._handle.zNear

    @z_near.setter
    def z_near(self, v: float):
        self._handle.zNear = _ctypes.c_float(v)

    @property
    def z_far(self) -> float:
        return self._handle.zFar

    @z_far.setter
    def z_far(self, v: float):
        self._handle.zFar = _ctypes.c_float(v)


class OrthographicCamera_:
    def __init__(self, handle: _CCameraOpts_Orthographic_):
        self._handle = handle

    @property
    def size(self) -> glm.vec2:
        return self._handle.size.to_glm()

    @size.setter
    def size(self, v: glm.vec2):
        self._handle.size.set_from_glm(v)

    @property
    def left(self) -> float:
        return self._handle.left

    @left.setter
    def left(self, v: float):
        self._handle.left = v

    @property
    def right(self) -> float:
        return self._handle.right

    @right.setter
    def right(self, v: float):
        self._handle.right = v

    @property
    def top(self) -> float:
        return self._handle.top

    @top.setter
    def top(self, v: float):
        self._handle.top = v

    @property
    def bottom(self) -> float:
        return self._handle.bottom

    @bottom.setter
    def bottom(self, v: float):
        self._handle.bottom = v


class Camera:
    def __init__(self, handle: _Ptr[_CCamera]):
        self._handle = handle

    @property
    def type(self) -> CameraType:
        return (
            CameraType.ORTHOGRAPHIC
            if self._handle[0].type == 0
            else CameraType.PERSPECTIVE
            if self._handle[0].type == 1
            else CameraType.UNKNOWN
        )

    @type.setter
    def type(self, value: CameraType):
        self._handle[0].type = value.value

    @property
    def position(self) -> glm.vec3:
        return self._handle[0].position.to_glm()

    @position.setter
    def position(self, value: glm.vec3):
        self._handle[0].position.set_from_glm(value)

    @property
    def rotation(self) -> glm.quat:
        return self._handle[0].rotation.to_glm()

    @rotation.setter
    def rotation(self, value: glm.quat):
        self._handle[0].rotation.set_from_glm(value)

    @property
    def perspective(self) -> PerspectiveCamera_:
        return PerspectiveCamera_(self._handle[0].opts.perspective)

    @perspective.setter
    def perspective(self, value: PerspectiveCamera_):
        self._handle[0].opts.perspective = value._handle

    @property
    def orthographic(self) -> OrthographicCamera_:
        return OrthographicCamera_(self._handle[0].opts.orthographic)

    @orthographic.setter
    def orthographic(self, value: OrthographicCamera_):
        self._handle[0].opts.orthographic = value._handle


class KeyCode(_enum.IntEnum):
    UNKNOWN = (-1,)
    SPACE = (32,)
    APOSTROPHE = (39,)
    COMMA = (44,)
    MINUS = (45,)
    PERIOD = (46,)
    SLASH = (47,)
    ALPHA0 = (48,)
    ALPHA1 = (49,)
    ALPHA2 = (50,)
    ALPHA3 = (51,)
    ALPHA4 = (52,)
    ALPHA5 = (53,)
    ALPHA6 = (54,)
    ALPHA7 = (55,)
    ALPHA8 = (56,)
    ALPHA9 = (57,)
    SEMI_COLON = (59,)
    EQUAL = (61,)
    A = (65,)
    B = (66,)
    C = (67,)
    D = (68,)
    E = (69,)
    F = (70,)
    G = (71,)
    H = (72,)
    I = (73,)
    J = (74,)
    K = (75,)
    L = (76,)
    M = (77,)
    N = (78,)
    O = (79,)
    P = (80,)
    Q = (81,)
    R = (82,)
    S = (83,)
    T = (84,)
    U = (85,)
    V = (86,)
    W = (87,)
    X = (88,)
    Y = (89,)
    Z = (90,)
    LEFT_BRACKET = (91,)
    BACKSLASH = (92,)
    RIGHT_BRACKET = (93,)
    GRAVE_ACCENT = (96,)
    WORLD1 = (161,)
    WORLD2 = (162,)
    ESCAPE = (256,)
    ENTER = (257,)
    TAB = (258,)
    BACKSPACE = (259,)
    INSERT = (260,)
    DELETE = (261,)
    RIGHT = (262,)
    LEFT = (263,)
    DOWN = (264,)
    UP = (265,)
    PAGE_UP = (266,)
    PAGE_DOWN = (267,)
    HOME = (268,)
    END = (269,)
    CAPS_LOCK = (280,)
    SCROLL_LOCK = (281,)
    NUM_LOCK = (282,)
    PRINT_SCREEN = (283,)
    PAUSE = (284,)
    F1 = (290,)
    F2 = (291,)
    F3 = (292,)
    F4 = (293,)
    F5 = (294,)
    F6 = (295,)
    F7 = (296,)
    F8 = (297,)
    F9 = (298,)
    F10 = (299,)
    F11 = (300,)
    F12 = (301,)
    F13 = (302,)
    F14 = (303,)
    F15 = (304,)
    F16 = (305,)
    F17 = (306,)
    F18 = (307,)
    F19 = (308,)
    F20 = (309,)
    F21 = (310,)
    F22 = (311,)
    F23 = (312,)
    F24 = (313,)
    F25 = (314,)
    NUMPAD0 = (320,)
    NUMPAD1 = (321,)
    NUMPAD2 = (322,)
    NUMPAD3 = (323,)
    NUMPAD4 = (324,)
    NUMPAD5 = (325,)
    NUMPAD6 = (326,)
    NUMPAD7 = (327,)
    NUMPAD8 = (328,)
    NUMPAD9 = (329,)
    NUMPAD_DECIMAL = (330,)
    NUMPAD_DIVIDE = (331,)
    NUMPAD_MULTIPLY = (332,)
    NUMPAD_SUBTRACT = (333,)
    NUMPAD_ADD = (334,)
    NUMPAD_ENTER = (335,)
    NUMPAD_EQUAL = (336,)
    LEFT_SHIFT = (340,)
    LEFT_CONTROL = (341,)
    LEFT_ALT = (342,)
    LEFT_SUPER = (343,)
    RIGHT_SHIFT = (344,)
    RIGHT_CONTROL = (345,)
    RIGHT_ALT = (346,)
    RIGHT_SUPER = (347,)
    MENU = 348


class Window:
    def __init__(self, size: glm.ivec2, title: str):
        self._handle = _C.gtamCreateWindow(
            _CVec2i(size.x, size.y), title.encode("utf-8")
        )
        self._check_errors()

    def _check_errors(self, msg: str | None = None):
        if _C.gtamGetError() != _GTAM_ERROR_NONE:
            raise Exception(
                f"{_gtam_error_to_text(_C.gtamGetError())}: {_C.gtamGetErrorMessage().decode('utf-8')}"
                + (" " + msg if msg is not None else "")
            )

    def init(self):
        _C.gtamInitWindow(self._handle)
        self._check_errors()

    def deinit(self):
        _C.gtamDeinitWindow(self._handle)
        self._check_errors()

    def close(self):
        _C.gtamCloseWindow(self._handle)

    def unclose(self):
        _C.gtamUncloseWindow(self._handle)

    def update(self, depth: bool = False):
        _C.gtamUpdateWindow(self._handle, 1 if depth else 0)
        self._check_errors()

    def is_key_down(self, key: KeyCode) -> bool:
        return not not _C.gtamWindowIsKeyDown(self._handle, key.value)

    def is_mouse_down(self, button: int) -> bool:
        return not not _C.gtamWindowIsMouseDown(self._handle, button)

    @property
    def time(self) -> float:
        return _C.gtamWindowGetTime(self._handle)

    @property
    def should_close(self) -> bool:
        return not not _C.gtamWindowShouldClose(self._handle)

    @property
    def framebuffer_size(self) -> glm.vec2:
        v = _CVec2()
        _C.gtamWindowGetFramebufferSize(self._handle, _ctypes.byref(v))
        return v.to_glm()

    @property
    def aspect_ratio(self) -> float:
        return _C.gtamWindowGetAspectRatio(self._handle)

    def get_mouse_position(self) -> glm.vec2:
        v = _CVec2()
        _C.gtamWindowGetMousePosition(self._handle, _ctypes.byref(v))
        return v.to_glm()

    def new_texture(self, path: str) -> Texture:
        ptr = _C.gtamWindowNewTexture(self._handle, path.encode("utf-8"))
        self._check_errors(path)
        return Texture(ptr)

    def new_sprite(self, texture: Texture, shader: Shader) -> Sprite:
        ptr = _C.gtamWindowNewSprite(self._handle, texture._handle, shader._handle)
        return Sprite(ptr)

    def new_shader(self, vertex: str, fragment: str, vertex_count: int) -> Shader:
        ptr = _C.gtamWindowNewShader(
            self._handle, vertex.encode("utf-8"), fragment.encode("utf-8"), vertex_count
        )
        self._check_errors(
            f"vertex: {vertex}, fragment: {fragment}, vertex_count: {vertex_count}"
        )
        return Shader(ptr)

    def new_camera(self, type: CameraType) -> Camera:
        ptr = _C.gtamWindowNewCamera(self._handle, type.value)
        self._check_errors()
        return Camera(ptr)

    def del_texture(self, texture: Texture):
        _C.gtamWindowDelTexture(self._handle, texture._handle)

    def del_sprite(self, sprite: Sprite):
        _C.gtamWindowDelSprite(self._handle, sprite._handle)

    def del_shader(self, shader: Shader):
        _C.gtamWindowDelShader(self._handle, shader._handle)

    def del_camera(self, camera: Camera):
        _C.gtamWindowDelCamera(self._handle, camera._handle)

    @property
    def active_camera(self):
        return Camera(_C.gtamWindowGetActiveCamera(self._handle))

    @active_camera.setter
    def active_camera(self, camera: Camera):
        _C.gtamWindowSetActiveCamera(self._handle, camera._handle)


__all__ = [
    "Camera",
    "Shader",
    "Texture",
    "TextureView",
    "Sprite",
    "Window",
    "CameraType",
    "KeyCode",
]
