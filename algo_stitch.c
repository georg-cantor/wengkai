#include <string.h>
#include "common.h"

#define MAX_INPUT_NUMS 4

//image transform varaibles
static gdouble g_homo_h[MAX_INPUT_NUMS][3][3] = {
{{0.6412965906640729630, -1.552528866208419567, 860.0910900339216596}, {0.1007221672987967109, -0.1623426289839597636, 316.0250091477126375}, {-0.0003109621658735104788, -0.0007737190760871934811, 1.0}}
,{{1.953621779030108696, -0.05149459238891157531, -1484.217891904689168}, {-0.4363034970023982884, -0.2355384757902673210, 999.6566151473966784}, {0.0004354045803305795653, -0.001457365011012365461, 1.0}}
,{{3.137256725424430570, -0.8607704151198101172, -1874.058173641584972}, {0.2072847845379020837, 0.06132110820063139955, 379.8407095907320468}, {0.00003992890840361172628, -0.001137775572983329344, 1.0}}
,{{0.00, 0.0, 0.00}, {0.00, 0.00, 0.00}, {0.00, 0.00, 0.00}}
};

//define input image's valid region: x_min < x < x_max && y_min < y < y_max
static gdouble g_img_valid_x_min[MAX_INPUT_NUMS] = {0.000, 0.000, 0.000, 0.000};
static gdouble g_img_valid_x_max[MAX_INPUT_NUMS] = {1.000, 1.000, 1.000, 1.000};
static gdouble g_img_valid_y_min[MAX_INPUT_NUMS] = {0.000, 0.000, 0.000, 0.000};
static gdouble g_img_valid_y_max[MAX_INPUT_NUMS] = {1.000, 1.000, 1.000, 1.000};

//LUT algorithm varaibles
static gint g_lut_w = 0;
static gint g_lut_h = 0;
static gint8   *g_lut_imgid = NULL; // -1 indicate no image
static guint16 *g_lut_x = NULL;     // x integral part
static guint16 *g_lut_y = NULL;     // y integral part
static guint8  *g_lut_fx = NULL;    // x decimal part. 8-bit
static guint8  *g_lut_fy = NULL;    // y decimal part. 8-bit
#define GET_LUT(lut,x,y)  (lut[(y)*g_lut_w+(x)])

static void image_transform(gdouble xin, gdouble yin, gdouble *xout, gdouble *yout, gint imgid)
{
/*
    Core function of stitching algorithm. To be futher improved by QinDongliang.
    命名规则请保持跟其他代码一致
    
    xin: dst image coordinate x.
    yin: dst image coordinate y.
    xout: pointer to src image coordinate x, need to be calculated.
    yout: pointer to src image coordinate y, need to be calculated.
    imgid:  src image id.
*/
/* This is a simple demo of homography function. */
    gdouble s = g_homo_h[imgid][2][0] * xin + g_homo_h[imgid][2][1] * yin + g_homo_h[imgid][2][2] * 1.0;
    *xout = (g_homo_h[imgid][0][0] * xin + g_homo_h[imgid][0][1] * yin + g_homo_h[imgid][0][2] * 1.0) / s;
    *yout = (g_homo_h[imgid][1][0] * xin + g_homo_h[imgid][1][1] * yin + g_homo_h[imgid][1][2] * 1.0) / s;
}

static gint algo_stitch_handle(
          ImageInfo *pii_in
        , ImageInfo *pii_out
        , gint nums
        , gpointer f_priv)
{
    gint dst_w = pii_out[0].width;
    gint dst_h = pii_out[0].height;
    gint dst_l = pii_out[0].linesize;
    guchar *dst_y = pii_out[0].buf;
    guchar *dst_u = dst_y + dst_l*dst_h;
    guchar *dst_v = dst_u + dst_l*dst_h/4;

    gint t;
    gint x, y; //image coordinates

    if (nums > MAX_INPUT_NUMS)
    {
        g_debug("Algo_Stitch Error. Input num(%d) exceed max input num(%d).\n", nums, MAX_INPUT_NUMS);
        nums = MAX_INPUT_NUMS;
    }
    
    //1. initialize LUT
    if (g_lut_w != dst_w || g_lut_h != dst_h)
    {
        g_debug("dst_l:%d. dst_w:%d. dst_h:%d.\n", dst_l, dst_w, dst_h);

        g_lut_w = dst_w;
        g_lut_h = dst_h;
        if (g_lut_imgid != NULL)
        {
            g_free(g_lut_imgid);
            g_free(g_lut_x);
            g_free(g_lut_y);
            g_free(g_lut_fx);
            g_free(g_lut_fy);
        }
        g_lut_imgid = (gint8*)  g_malloc(sizeof(gint8)  *g_lut_w*g_lut_h);
        g_lut_x     = (guint16*)g_malloc(sizeof(guint16)*g_lut_w*g_lut_h);
        g_lut_y     = (guint16*)g_malloc(sizeof(guint16)*g_lut_w*g_lut_h);
        g_lut_fx    = (guint8*) g_malloc(sizeof(guint8) *g_lut_w*g_lut_h);
        g_lut_fy    = (guint8*) g_malloc(sizeof(guint8) *g_lut_w*g_lut_h);

        //calculate LUT
        memset(g_lut_imgid, -1, sizeof(gint8)*g_lut_w*g_lut_h);
        for (t=0; t<nums; t++)
        {
            gint src_w = pii_in[t].width;
            gint src_h = pii_in[t].height;
			gint x_min = MAX(      0, src_w*g_img_valid_x_min[t]);
			gint x_max = MIN(src_w-1, src_w*g_img_valid_x_max[t]);
			gint y_min = MAX(      0, src_h*g_img_valid_y_min[t]);
			gint y_max = MIN(src_h-1, src_h*g_img_valid_y_max[t]);
	        for (y=0; y<g_lut_h; y++)
	        {
		        for (x=0; x<g_lut_w; x++)
		        {
		            gdouble xout, yout;
                    image_transform((gdouble)(x), (gdouble)(y), &xout, &yout, t);

                    if (xout > x_min && xout < x_max && yout > y_min && yout < y_max)
		            {
		                GET_LUT(g_lut_imgid,x,y) = t;
		                GET_LUT(g_lut_x,x,y) = (guint16)(xout);
		                GET_LUT(g_lut_y,x,y) = (guint16)(yout);
		                GET_LUT(g_lut_fx,x,y) = (guint8)(255*(xout-(guint16)(xout))); // 8 bit store
		                GET_LUT(g_lut_fy,x,y) = (guint8)(255*(yout-(guint16)(yout)));
		            }
		        }
	        }
	    }
    }

    //2. run LUT algorithm

    //2.1 initialize black image
    memset(dst_y, 0, dst_l*dst_h);
    memset(dst_u, 128, dst_l*dst_h/4);
    memset(dst_v, 128, dst_l*dst_h/4);

    guchar *src_y[MAX_INPUT_NUMS];
    guchar *src_u[MAX_INPUT_NUMS];
    guchar *src_v[MAX_INPUT_NUMS];
    for (t=0; t<nums; t++)
    {
        src_y[t] = pii_in[t].buf;
        src_u[t] = src_y[t] + pii_in[t].linesize * pii_in[t].height;
        src_v[t] = src_u[t] + pii_in[t].linesize * pii_in[t].height / 4;
    }

    //2.2
    for (y=0; y<g_lut_h; y++)
    {
        for (x=0; x<g_lut_w; x++)
        {
            t = GET_LUT(g_lut_imgid,x,y);
            
            if (t >=0 && t < nums)
            {
                guint16 x0 = GET_LUT(g_lut_x,x,y);
                guint16 y0 = GET_LUT(g_lut_y,x,y);
                guint fx1 = GET_LUT(g_lut_fx,x,y);
                guint fy1 = GET_LUT(g_lut_fy,x,y);
                guint fx0 = 256-fx1;
                guint fy0 = 256-fy1;

#define SRC_W(t) (pii_in[t].width)
#define SRC_H(t) (pii_in[t].height)
#define SRC_L(t) (pii_in[t].linesize)
#define SRC_Y(t,x,y) (src_y[t][(y)*SRC_L(t)+(x)])
#define SRC_U(t,x,y) (src_u[t][(y)*SRC_L(t)/2+(x)])
#define SRC_V(t,x,y) (src_v[t][(y)*SRC_L(t)/2+(x)])

                guint pixel00, pixel01, pixel10, pixel11;
                pixel00 = SRC_Y(t, x0  , y0  );
                pixel01 = SRC_Y(t, x0+1, y0  );
                pixel10 = SRC_Y(t, x0  , y0+1);
                pixel11 = SRC_Y(t, x0+1, y0+1);
                
                dst_y[y*dst_l+x] = (guchar) ((fy0*(fx0*pixel00 + fx1*pixel01) + fy1*(fx0*pixel10 + fx1*pixel11))>>16);
                
                if (y%2 == 0 && x%2 == 0) //do u & v
                {
                    fx1 = (fx1+256*(x0%2)) / 2;
                    fy1 = (fy1+256*(y0%2)) / 2;
                    fx0 = 256-fx1;
                    fy0 = 256-fy1;
                    guint16 x0_uv = x0/2;
                    guint16 y0_uv = y0/2;

                    pixel00 = SRC_U(t, x0_uv  , y0_uv  );
                    pixel01 = SRC_U(t, x0_uv+1, y0_uv  );
                    pixel10 = SRC_U(t, x0_uv  , y0_uv+1);
                    pixel11 = SRC_U(t, x0_uv+1, y0_uv+1);
                    dst_u[y/2*dst_l/2+x/2] = (guchar) ((fy0*(fx0*pixel00 + fx1*pixel01) + fy1*(fx0*pixel10 + fx1*pixel11))>>16);

                    pixel00 = SRC_V(t, x0_uv  , y0_uv  );
                    pixel01 = SRC_V(t, x0_uv+1, y0_uv  );
                    pixel10 = SRC_V(t, x0_uv  , y0_uv+1);
                    pixel11 = SRC_V(t, x0_uv+1, y0_uv+1);
                    dst_v[y/2*dst_l/2+x/2] = (guchar) ((fy0*(fx0*pixel00 + fx1*pixel01) + fy1*(fx0*pixel10 + fx1*pixel11))>>16);
                }
            }
        }
    }
    
    return FILTER_SWAP;
}

gint algo_stitch_open(gpointer *f_priv)
{
    g_lut_w = 0;
    g_lut_h = 0;
    g_lut_imgid = NULL;
    return TRUE;
}

void algo_stitch_close(gpointer f_priv)
{
    if (g_lut_imgid != NULL)
    {
        g_free(g_lut_imgid);
        g_free(g_lut_x);
        g_free(g_lut_y);
        g_free(g_lut_fx);
        g_free(g_lut_fy);
    }
}

Filter algo_stitch_filter = {
    .name = "algo_stitch_filter",
    .func_set = {
        .filter_open = algo_stitch_open,
        .filter_close = algo_stitch_close,
        .filter_handle = algo_stitch_handle,
    },
};

gint algo_stitch_init(void)
{
    gint ret = filter_register(IDS_TYPE_STITCH, &algo_stitch_filter);
    if (ret < 0) {
        g_error("algo_stitch_filter register failed with error code:%d\n", ret);
        return FALSE;
    }
    
    return TRUE;
}

