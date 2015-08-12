#macro mkPipe(a,b)
	cylinder
	{
		a,b,0.01
	}

#end

#macro mkNode(a)
	sphere
	{
		a,0.03
	}
#end

#macro mkCube(m,p,c)

union
{
	mkPipe(<m.x,m.y,m.z>,<p.x,m.y,m.z>)
	mkPipe(<p.x,m.y,m.z>,<p.x,p.y,m.z>)
	mkPipe(<p.x,p.y,m.z>,<m.x,p.y,m.z>)
	mkPipe(<m.x,p.y,m.z>,<m.x,m.y,m.z>)

	mkPipe(<m.x,m.y,m.z>,<m.x,m.y,p.z>)
	mkPipe(<p.x,m.y,m.z>,<p.x,m.y,p.z>)
	mkPipe(<p.x,p.y,m.z>,<p.x,p.y,p.z>)
	mkPipe(<m.x,p.y,m.z>,<m.x,p.y,p.z>)

	mkPipe(<m.x,m.y,p.z>,<p.x,m.y,p.z>)
	mkPipe(<p.x,m.y,p.z>,<p.x,p.y,p.z>)
	mkPipe(<p.x,p.y,p.z>,<m.x,p.y,p.z>)
	mkPipe(<m.x,p.y,p.z>,<m.x,m.y,p.z>)

	mkNode(<m.x,m.y,m.z>)
	mkNode(<m.x,m.y,p.z>)
	mkNode(<m.x,p.y,m.z>)
	mkNode(<m.x,p.y,p.z>)
	mkNode(<p.x,m.y,m.z>)
	mkNode(<p.x,m.y,p.z>)
	mkNode(<p.x,p.y,m.z>)
	mkNode(<p.x,p.y,p.z>)

	pigment { color c }
}
#end

mkCube(<-1,-1,-1>,<1,1,1>,rgb <1,1,1>)

camera
{
	right -1.33*x
	sky <0,0,1>
	
	location <3,2,1.5>
	look_at <0,0,0>	
}

light_source
{
	<20,30,20> color rgb <1,1,1>
}
